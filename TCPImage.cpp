#define ASIO_STANDALONE

#include <asio.hpp>
#include <iostream>
#include <fstream>

using asio::ip::tcp;

int main(int argc, char* argv[]){
    bool set = false;
    std::ofstream img("img.jpg", std::ofstream::out);
    try
    {
        asio::io_service io_service;
        tcp::resolver resolver(io_service);

        tcp::socket socket(io_service);
        std::cout << "Begin!" << std::endl;
        asio::connect(socket, resolver.resolve({"10.120.88.80","2420"}));
        //asio::connect(socket, resolver.resolve({"127.0.0.1","5005"}));
        std::cout << "Connected!" << std::endl;

        for(;;){
            char buf[150000];
            asio::error_code error;
            char snd[128] = "Hello World";
            size_t reqlen = std::strlen(snd);
            asio::write(socket, asio::buffer(snd, reqlen));
            std::cout << "Written!" << std::endl;
            char len[4] = "";
            //socket.read_some(asio::buffer(len, 10), error);
            //for(int i = 0; len[i] != '\0' || i < 10; i++){
              //  std::cout << len[i] << ", ";
            //}

            size_t sz = asio::read(socket, asio::buffer(len, 4));
            int l = 0;
            for(size_t i = 0; i < sz; i++){
                l += ((int)len[i]) << (8*i);
            }
            //std::cout << len[i] << " ";
            //std::cout << std::endl;
            std::cout << l << std::endl;
            size_t msglen = asio::read(socket, asio::buffer(buf, l));
            //size_t msglen = socket.read_some(asio::buffer(buf, l), error);
            std::cout << msglen << std::endl;;
            if(error == asio::error::eof)
                break;
            else if(error)
                throw asio::system_error(error);

            std::cout << "Msg! ";
            //std::cout.write(buf, msglen);
            std::cout << "\n";
            if(!set){
                for(int i = 0; i < msglen; i++){
                    img << buf[i];
                }
                set = true;
                std::cout << "BREAK!" << std::endl;
                break;
            } else break;
        }
    }
    catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }
    std::cout << "exit" << std::endl;
    img.close();
}
