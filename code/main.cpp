#include "server_interface.h"
#include "stream.h"
#include "zhidao.h"
int main(int argc, char *argv[])
{
    ZhiDao zhidao;
    zhidao.init("/home/y/github/scrcpy-GamePro/res/template.jpg");
    ServerInterface server;
    server.init();
    Stream video_stream;
    video_stream.stream_init(server.get_socket());

    Mat src;
    while(1){
        if(!video_stream.get_img(src))
            break;
        imshow("origin",src);
        if(!zhidao.detect(src)){
            zhidao.action();
            cout << "action" << endl;
        }
        waitKey(1);
    }
    return 0;
}

