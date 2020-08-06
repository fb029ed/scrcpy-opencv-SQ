#include "server_interface.h"
#include "stream.h"
int main(int argc, char *argv[])
{
    ServerInterface server;
    server.init();
    Stream video_stream;
    video_stream.stream_init(server.get_socket());

    Mat src;
    while(1){
        if(!video_stream.get_img(src))
            break;
        imshow("origin",src);
        waitKey(1);
    }
    return 0;
}

