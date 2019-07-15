package com.xk.ffmpegdemp.player;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * @author xuekai1
 * @date 2019-07-03
 */
public class MPlayer implements SurfaceHolder.Callback {
    static {
        System.loadLibrary("myffmpeg");
    }

    private final SurfaceView surfaceView;
    private String dataSource;
    private String cookie;
    private MPlayerListener mPlayerListener;
    private final SurfaceHolder holder;

    public MPlayer(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
        holder = surfaceView.getHolder();
        holder.addCallback(this);
    }

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    public void setCookie(String cookie) {
        this.cookie = cookie;
    }

    public void prepare() {
        nprepare(dataSource, cookie == null ? "" : cookie);
    }

    public void setListener(MPlayerListener mPlayerListener) {
        this.mPlayerListener = mPlayerListener;
    }

    public void play() {
        nstart();
    }

    public void release() {
        holder.removeCallback(this);

    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Surface surface = holder.getSurface();
        nsetSurface(surface);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
    }

    public interface MPlayerListener {
        void onError(int errorCode, String msg);

        void onPrepare();

        void onPlay();
    }


    //↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓被native调用的方法
    private void onError(int errorCode, String msg) {
        mPlayerListener.onError(errorCode, msg);
    }

    private void onPrepare() {
        mPlayerListener.onPrepare();
    }

    //↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓native方法
    native void nprepare(String dataSource, String cookie);

    native void nstart();

    native void nsetSurface(Surface surface);
}
