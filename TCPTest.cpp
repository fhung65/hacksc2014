#define ASIO_STANDALONE

#include <asio.hpp>
#include <iostream>

using asio::ip::tcp;

int main(int argc, char* argv[]){
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
            char buf[128];
            asio::error_code error;
            char snd[128] = "Hello World";
            size_t reqlen = std::strlen(snd);
            asio::write(socket, asio::buffer(snd, reqlen));
            std::cout << "Written!" << std::endl;
            char len[4] = "";
            socket.read_some(asio::buffer(len, 4), error);
            int l = atoi(len);
            std::cout << l << std::endl;
            size_t msglen = socket.read_some(asio::buffer(buf, l), error);
            std::cout << "here?";
            if(error == asio::error::eof)
                break;
            else if(error)
                throw asio::system_error(error);

            std::cout << "Msg! ";
            std::cout.write(buf, msglen);
            std::cout << "\n";
        }
    }
    catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }
}
