#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>
#include <fftw3.h>

template<class T>
void fft(std::vector<T>& in, std::vector<std::complex<T>>& out);

template<>
void fft<float>(std::vector<float>& in, std::vector<std::complex<float>>& out) {
    out.resize(in.size());
    auto plan = fftwf_plan_dft_r2c_1d(in.size(),
                                      in.data(),
                                      reinterpret_cast<fftwf_complex*>(out.data()),
                                      FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);
}

template<>
void fft<double>(std::vector<double>& in, std::vector<std::complex<double>>& out) {
    out.resize(in.size());
    auto plan = fftw_plan_dft_r2c_1d(in.size(),
                                     in.data(),
                                     reinterpret_cast<fftw_complex*>(out.data()),
                                     FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

template<class T>
void ifft(std::vector<std::complex<T>>& in, std::vector<T>& out);

template<>
void ifft<float>(std::vector<std::complex<float>>& in, std::vector<float>& out) {
    out.resize(in.size());
    auto plan = fftwf_plan_dft_c2r_1d(in.size(),
                                      reinterpret_cast<fftwf_complex*>(in.data()),
                                      out.data(),
                                      FFTW_ESTIMATE);
    fftwf_execute(plan);
    fftwf_destroy_plan(plan);
}

template<>
void ifft<double>(std::vector<std::complex<double>>& in, std::vector<double>& out) {
    out.resize(in.size());
    auto plan = fftw_plan_dft_c2r_1d(in.size(),
                                     reinterpret_cast<fftw_complex*>(in.data()),
                                     out.data(),
                                     FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);
}

#endif // FFT_H
