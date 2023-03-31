# wasm_playground
 
Step 1, download Emscripten and install it.
https://emscripten.org/docs/getting_started/downloads.html

Step 2. Compile
cd wasm_playground
emcc -g -s ENVIRONMENT='web' -std=c++20 -lembind fanvacool.cpp -o fanvacool.html -s ASYNCIFY -s USE_SDL=2 -s USE_SDL_GFX=2

Step 3. Serve
python3 -m http.server --directory ./wasm_playground 8000

Step 4. open http://127.0.0.1:8000/fanvacool.html 

Step 5. Enjoy


# Resources:
https://web.dev/drawing-to-canvas-in-emscripten/