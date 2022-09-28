#ifndef HANDLEOBSERVER_H
#define HANDLEOBSERVER_H

class HandleObserver {
public:
    void onMainHandleMoved();

    /**
     * @brief onMainHandleMoved
     * @param fIndex the frequency index
     * @param y Gain value for peaking filter, otherwise Q index
     */
    virtual void onMainHandleMoved(int fIndex, double y) = 0;

    /**
     * @brief onLeftHandleMoved
     * @param qIndex
     */
    virtual void onLeftHandleMoved(double qIndex);

    /**
     * @brief onRightHandleMoved
     * @param qIndex
     */
    virtual void onRightHandleMoved(double qIndex);
};

#endif // HANDLEOBSERVER_H
