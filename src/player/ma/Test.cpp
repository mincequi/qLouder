#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <stdio.h>

/* Data Format */
#define FORMAT              ma_format_f32   /* Must always be f32. */
#define CHANNELS            2
#define SAMPLE_RATE         48000

/* Effect Properties */
#define LPF_BIAS            0.9f    /* Higher values means more bias towards the low pass filter (the low pass filter will be more audible). Lower values means more bias towards the echo. Must be between 0 and 1. */
#define LPF_CUTOFF_FACTOR   80      /* High values = more filter. */
#define LPF_ORDER           8
#define DELAY_IN_SECONDS    0.2f
#define DECAY               0.5f    /* Volume falloff for each echo. */

typedef struct
{
    ma_data_source_node node;   /* If you make this the first member, you can pass a pointer to this struct into any ma_node_* API and it will "Just Work". */
    ma_decoder decoder;
} sound_node;

static ma_node_graph    g_nodeGraph;
static ma_lpf_node      g_lpfNode;
static ma_delay_node    g_delayNode;
static ma_splitter_node g_splitterNode;
static sound_node*      g_pSoundNodes;
static int              g_soundNodeCount;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    MA_ASSERT(pDevice->playback.channels == CHANNELS);

    /*
    Hearing the output of the node graph is as easy as reading straight into the output buffer. You just need to
    make sure you use a consistent data format or else you'll need to do your own conversion.
    */
    ma_node_graph_read_pcm_frames(&g_nodeGraph, pOutput, frameCount, NULL);

    (void)pInput;   /* Unused. */
}

int main(int argc, char** argv)
{
    int iarg;
    ma_result result;

    ma_node_graph_config nodeGraphConfig = ma_node_graph_config_init(CHANNELS);
    result = ma_node_graph_init(&nodeGraphConfig, NULL, &g_nodeGraph);

    ma_lpf_node_config lpfNodeConfig = ma_lpf_node_config_init(CHANNELS, SAMPLE_RATE, SAMPLE_RATE / LPF_CUTOFF_FACTOR, LPF_ORDER);
    result = ma_lpf_node_init(&g_nodeGraph, &lpfNodeConfig, NULL, &g_lpfNode);


    /* Connect the output bus of the low pass filter node to the input bus of the endpoint. */
    ma_node_attach_output_bus(&g_lpfNode, 0, ma_node_graph_get_endpoint(&g_nodeGraph), 0);


    /* Echo / Delay. */
    {
        ma_delay_node_config delayNodeConfig = ma_delay_node_config_init(CHANNELS, SAMPLE_RATE, (ma_uint32)(SAMPLE_RATE * DELAY_IN_SECONDS), DECAY);
        result = ma_delay_node_init(&g_nodeGraph, &delayNodeConfig, NULL, &g_delayNode);

        /* Connect the output bus of the delay node to the input bus of the endpoint. */
        ma_node_attach_output_bus(&g_delayNode, 0, ma_node_graph_get_endpoint(&g_nodeGraph), 0);

        /* Set the volume of the delay filter to make it more of less impactful. */
        ma_node_set_output_bus_volume(&g_delayNode, 0, 1 - LPF_BIAS);
    }

    /* Splitter. */
    {
        ma_splitter_node_config splitterNodeConfig = ma_splitter_node_config_init(CHANNELS);

        result = ma_splitter_node_init(&g_nodeGraph, &splitterNodeConfig, NULL, &g_splitterNode);

        /* Connect output bus 0 to the input bus of the low pass filter node, and output bus 1 to the input bus of the delay node. */
        ma_node_attach_output_bus(&g_splitterNode, 0, &g_lpfNode,   0);
        ma_node_attach_output_bus(&g_splitterNode, 1, &g_delayNode, 0);
    }

    /* Data sources. Ignore any that cannot be loaded. */
    g_pSoundNodes = (sound_node*)ma_malloc(sizeof(*g_pSoundNodes) * argc-1, NULL);

    g_soundNodeCount = 0;
    for (iarg = 1; iarg < argc; iarg += 1) {
        ma_decoder_config decoderConfig = ma_decoder_config_init(FORMAT, CHANNELS, SAMPLE_RATE);

        result = ma_decoder_init_file(argv[iarg], &decoderConfig, &g_pSoundNodes[g_soundNodeCount].decoder);
        if (result == MA_SUCCESS) {
            ma_data_source_node_config dataSourceNodeConfig = ma_data_source_node_config_init(&g_pSoundNodes[g_soundNodeCount].decoder);

            result = ma_data_source_node_init(&g_nodeGraph, &dataSourceNodeConfig, NULL, &g_pSoundNodes[g_soundNodeCount].node);
            if (result == MA_SUCCESS) {
                /* The data source node has been created successfully. Attach it to the splitter. */
                ma_node_attach_output_bus(&g_pSoundNodes[g_soundNodeCount].node, 0, &g_splitterNode, 0);
                g_soundNodeCount += 1;
            }
        }
    }

    /* Everything has been initialized successfully so now we can set up a playback device so we can listen to the result. */
    {
        ma_device_config deviceConfig;
        ma_device device;

        deviceConfig = ma_device_config_init(ma_device_type_playback);
        deviceConfig.playback.format   = FORMAT;
        deviceConfig.playback.channels = CHANNELS;
        deviceConfig.sampleRate        = SAMPLE_RATE;
        deviceConfig.dataCallback      = data_callback;
        deviceConfig.pUserData         = NULL;

        result = ma_device_init(NULL, &deviceConfig, &device);
        result = ma_device_start(&device);

        printf("Press Enter to quit...\n");
        getchar();

        /* We're done. Clean up the device. */
        ma_device_uninit(&device);
    }


cleanup_graph:
    {
        /* It's good practice to tear down the graph from the lowest level nodes first. */
        int iSound;

        /* Sounds. */
        for (iSound = 0; iSound < g_soundNodeCount; iSound += 1) {
            ma_data_source_node_uninit(&g_pSoundNodes[iSound].node, NULL);
            ma_decoder_uninit(&g_pSoundNodes[iSound].decoder);
        }

        /* Splitter. */
        ma_splitter_node_uninit(&g_splitterNode, NULL);

        /* Echo / Delay */
        ma_delay_node_uninit(&g_delayNode, NULL);

        /* Low Pass Filter */
        ma_lpf_node_uninit(&g_lpfNode, NULL);

        /* Node Graph */
        ma_node_graph_uninit(&g_nodeGraph, NULL);
    }

    return 0;
}
