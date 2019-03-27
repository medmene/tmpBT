// dear imgui: standalone example application for SDL2 + OpenGL
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_sdl_opengl3/ folder**
// See imgui_impl_sdl.cpp for details.

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "exprtk.hpp"
#include <string>

using std::string;

template <typename T>
double trig_function(string func, double _x)
{
	typedef exprtk::symbol_table<T> symbol_table_t;
	typedef exprtk::expression<T>     expression_t;
	typedef exprtk::parser<T>             parser_t;

	T x;
	symbol_table_t symbol_table;
	symbol_table.add_variable("x", x);
	symbol_table.add_constants();

	expression_t expression;
	expression.register_symbol_table(symbol_table);

	parser_t parser;
	parser.compile(func, expression);

	x = _x;
	return expression.value();
}

double Dichotomy(string expr, double a, double b, double eps, double& extrX)
{	
	double f_a = trig_function<double>(expr, a);
	double f_b = trig_function<double>(expr, b);

	while (b - a > eps){
		double xMid = (a + b) / 2;
		double leftMid = trig_function<double>(expr, (xMid - eps));
		double rightMid = trig_function<double>(expr, (xMid + eps));
		
		if (leftMid < rightMid){
			b = xMid;
			f_b = trig_function<double>(expr, b);
		}
		else{
			a = xMid;
			f_a = trig_function<double>(expr, a);
		}
	}

	extrX = a;
	return trig_function<double>(expr, extrX);
}

double GoldenRatio(string expr, double a, double b, double eps, double & extrX)
{
	const double GRatio = 0.381966011;
	double x1 = a + GRatio * (b - a);
	double x2 = b - GRatio * (b - a);
	double f_x1 = trig_function<double>(expr, x1);
	double f_x2 = trig_function<double>(expr, x2);
	double f_a = trig_function<double>(expr, a);
	double f_b = trig_function<double>(expr, b);

	while (b - a > eps){
		if (f_x1 < f_x2){
			b = x2;
			f_b = f_x2;
			x2 = x1;
			f_x2 = f_x1;
			x1 = a + GRatio * (b - a);
			f_x1 = trig_function<double>(expr, x1);
		}
		else{
			a = x1;
			f_a = f_x1;
			x1 = x2;
			f_x1 = f_x2;
			x2 = b - GRatio * (b - a);
			f_x2 = trig_function<double>(expr, x2);
		}
	}

	extrX = a;
	return  trig_function<double>(expr, extrX);
}

int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();
	    
    io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 18.0f);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();


        {
			bool actv1 = true, actv2 = false;
			char func[128];
			char res[512];
			double a = 0, b = 0;
			double xRes = 0, yRes = 0;

            ImGui::Begin("Caclulator");
			
			ImGui::Text("Minimize func:");
			ImGui::InputText("", func, 128);
			ImGui::Text("Interval:");
			ImGui::InputDouble("a=", &a);
			ImGui::InputDouble("b=", &b);

			ImGui::Text("Optimisation method:");
			actv1 = ImGui::RadioButton("Method Dihtomy", actv1);
			actv2 = ImGui::RadioButton("Method Golden ratio", actv2);
			if (ImGui::Button("Calculate")) {
				if (string(func).size() != 0 && a < b) {
					if (actv1 && !actv2) {
						yRes = Dichotomy(string(func), a, b, 0.001, xRes);
						string t = string(res) + "Method Dihtomy:\n  Min in: x: " +
							std::to_string(xRes) + " x: " + std::to_string(yRes) + "\n";
						strcpy(res, t.c_str());
					}
					else if (actv2 && !actv1) {
						yRes = GoldenRatio(string(func), a, b, 0.001, xRes);
						string t = string(res) + "Method Golden ratio:\n  Min in: x: " +
							std::to_string(xRes) + " y: " + std::to_string(yRes) + "\n";
						strcpy(res, t.c_str());
					}
				}					
			}

			ImGui::InputTextMultiline("Results:", res, 512, ImVec2(200, 200));
            
            ImGui::End();
        }
		      

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
