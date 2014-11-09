#define ASIO_STANDALONE

#include <asio.hpp>
#include <iostream>
#include <fstream>
#include "Peripherals.h"
#include <SDL.h>
#include <SDL_image.h>

using asio::ip::tcp;

int main(int argc, char* argv[]){
    bool set = false;
	peripherals per;
    try
    {
        asio::io_service io_service;
        tcp::resolver resolver(io_service);

        tcp::socket socket(io_service);
        std::cout << "Begin!" << std::endl;
        asio::connect(socket, resolver.resolve({"10.120.88.80","2420"}));
        //asio::connect(socket, resolver.resolve({"127.0.0.1","5005"}));
        std::cout << "Connected!" << std::endl;

		///////////////////////////////////////////////////Setup SDL
		const int wind_width = 640;
		const int wind_height = 480;

		SDL_Init(SDL_INIT_VIDEO);
		SDL_Window* wind = SDL_CreateWindow("tankVision", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			wind_width, wind_height,0);
		
		SDL_Renderer* wind_rend = SDL_GetRenderer(wind);

		SDL_Surface* wind_surf = SDL_GetWindowSurface(wind);
		SDL_FillRect(wind_surf, NULL, SDL_MapRGB(wind_surf->format, 0xff, 0xff, 0xff));
		SDL_UpdateWindowSurface(wind);

		SDL_RWops* rwops = nullptr;
		SDL_Surface* load_surf = nullptr;

		///////////////////////////////////////////////////end setup SDL

		for (;;){ //main loop
			std::cout << "!";
			char buf[500000];
			asio::error_code error;
			char len[5] = "";
			//socket.read_some(asio::buffer(len, 10), error);
			//for(int i = 0; len[i] != '\0' || i < 10; i++){
			//  std::cout << len[i] << ", ";
			//}

			size_t sz = asio::read(socket, asio::buffer(len, 4)); //get size of image
			int l = 0;
			/* std::cout << "hi" << std::endl;
			 std::cout << (int)len[0] << std::endl;
			 std::cout << (int)len[1] << std::endl;
			 std::cout << (int)len[2] << std::endl;
			 std::cout << (int)len[3] << std::endl;*/
			/* for(size_t i = 0; i < sz; i++){
				 l += ((unsigned int)(len[i]) << (8*i));
				 }*/

			l += (((int)len[0] & (0xfe)) >> 1);
			l += (((int)len[1] & (0xfe)) << 6);
			l += (((int)len[2] & (0xfe)) << 13);
			l += (((int)len[3] & (0xfe)) << 20); //hackish nonsense
			//std::cout << len[i] << " ";
			//std::cout << std::endl;
			std::cout << l << std::endl;
			size_t msglen = -1;
			if (l > 0) {
				std::cout << "l > 0" << std::endl;
				msglen = asio::read(socket, asio::buffer(buf, l)); //get image
			}

			/* while(msglen < (l-1000)){
				 msglen += asio::read(socket, asio::buffer((buf+msglen), 1000));
				 //std::cout << msglen << std::endl;
				 }
				 msglen += asio::read(socket, asio::buffer((buf+msglen), l-msglen));*/
			//size_t msglen = socket.read_some(asio::buffer(buf, l), error);
			std::cout << msglen << std::endl;
			if (error == asio::error::eof)
				break;
			else if (error)
				throw asio::system_error(error);


			//std::cout << "Msg! ";
			//std::cout.write(buf, msglen);
			//std::cout << "\n";
			if (msglen != -1)
			{
				//std::cout << "Img" << std::endl;
				//std::ofstream img("img.jpg", std::ofstream::binary | std::ofstream::out);
				//for (int i = 0; i < msglen; i++){
				//	img << buf[i];



				SDL_SetRenderDrawColor(wind_rend, 0xff, 0xff, 0xff, 0xff);
				SDL_RenderClear(wind_rend);

				//rwops = SDL_RWFromFile("img.jpg", "rb");
				rwops = SDL_RWFromMem(buf, msglen);
				load_surf = IMG_LoadJPG_RW(rwops);
				SDL_BlitSurface(load_surf, NULL, wind_surf, NULL);
				SDL_UpdateWindowSurface(wind);
				//img.close();
			}

		
/*            if(!set){
                for(int i = 0; i < msglen; i++){
                    img << buf[i];
                }
                set = true;
                std::cout << "BREAK!" << std::endl;
                break;
            } else break;*/
			std::string out = per.loop();
			size_t outlen = out.length();
			char snd[128]="";
			strncpy_s(snd,out.c_str(),outlen);
			//std::cout << snd << std::endl;
			size_t reqlen = std::strlen(snd);
			snd[reqlen] = '\0';
			asio::write(socket, asio::buffer(snd, reqlen+1));
			std::cout << "Written!" << std::endl;
        }
		//End of main loop
		
		SDL_FreeSurface(load_surf);
		load_surf = nullptr;
		SDL_FreeSurface(wind_surf);
		wind_surf = nullptr;
		SDL_DestroyWindow(wind);
		wind = nullptr;

    }
    catch (std::exception& e){
        std::cerr << e.what() << std::endl;
    }

	SDL_Quit();
    std::cout << "exit" << std::endl;
}
