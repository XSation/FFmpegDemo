package com.xk.ffmpegdemp;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;
import android.view.View;

import com.xk.ffmpegdemp.player.MPlayer;

public class MainActivity extends AppCompatActivity {


    private SurfaceView surfaceView;
    private MPlayer mPlayer;


    String[] dataSource = {
            "rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov",
            "http://tj-btfs-v2.yun.ftn.qq.com/ftn_handler/54e53ae183d30fde1d8bc1a06043272ffd53ee76f521ff71e665c2f5737c395e9d8e50a209fa403a79a7b1d5de4f883436c9c7802f92a1fd952e827a5fc45602",
            "http://tj-btfs-v2.yun.ftn.qq.com/ftn_handler/24ee9495ce368ad4f8e5c6502d1d676a2b0002ff4dd9001d68a944c6a40e2c84070fe051e962dde681e06f756d21750176c7745d230e975f524b2df7a3e0f946",
            "rtmp://pull-g.kktv8.com/livekktv/100987038"
    };
    String[] cookie = {
            "",
            "FTN5K=5df86d47",
            "FTN5K=d3996990",
            ""
    };
    //12、破冰
    int index = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        surfaceView = findViewById(R.id.surfaceView);
        mPlayer = new MPlayer(surfaceView);

        mPlayer.setDataSource(dataSource[index]);
        mPlayer.setCookie(cookie[index]);
        mPlayer.setListener(new MPlayer.MPlayerListener() {
            @Override
            public void onError() {

            }

            @Override
            public void onPrepare() {

            }

            @Override
            public void onPlay() {

            }
        });
    }


    public void play(View view) {
        mPlayer.prepare();
//        mPlayer.play();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mPlayer.release();
    }
}
