#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Point center = {.x = 100, .y = 100};
const int radius = 100;

int xres = 800; int yres = 600;
#define PI 3.14159
double klock()
{
  static double taim = 0;
  taim += 1.0;
  return taim;
}
double normRandom()
{
  return rand() / static_cast<double>(RAND_MAX-1);
}
#define rnd normRandom()


double normRandomMinus1to1()
{
  return -1.0 + 2.0 * rnd;
}
#define nrnd normRandomMinus1to1()

static int numframes = 0;
void balls_to_the_walls()
{
  enum {N=16384/4,NS=3};
  static double px[N], py[N], pvx[N], pvy[N], pr[N], pg[N], pb[N], prad[N];
  static double clut[NS], slut[NS], frad;
  static double tim = 0, hx, hy, hz, rhz;

  if (numframes == 0)
  {
     for(int i=0;i<N;i++)
     {
        px[i] = xres*rnd; pvx[i] = 4*nrnd;//*64;
        py[i] = yres*rnd; pvy[i] = 4*nrnd;//*64;
        pr[i] = (int(64*rnd)+0x60)/256.;
        pg[i] = (int(64*rnd)+0x60)/256.;
        pb[i] = (int(64*rnd)+0x60)/256.;
        prad[i] = 10.*rnd+.1;
     }
     double r = 1.0/cos(PI/(double)NS);
     for(int i=0;i<NS;i++)
     {
        clut[i] = cos((i+.5)*PI*2.0/NS-PI/2.0)*r;
        slut[i] = sin((i+.5)*PI*2.0/NS-PI/2.0)*r;
     }
     hx = xres/2.0; hy = yres/2.0; hz = hy; rhz = 1.0/hz;
  }

  double otim = tim; tim = klock(); 
  double dtim = tim-otim;
  char buf[256];
  for(int i=N-1;i>=0;i--)
  {
     double r = prad[i];
     double x = (px[i]);
     double y = (py[i]);
    //  double x = (px[i]-hx)*rhz;
    //  double y = (py[i]-hy)*rhz;
     unsigned char cr = int(255*pr[i]);
     unsigned char cg = int(255*pg[i]);
     unsigned char cb = int(255*pb[i]);
     //sprintf(buf, "#%x%x%x", cr,cg,cb);

    filledCircleRGBA(renderer, x, y, r,
                   /* RGBA: green */ cr, cg, cb, 0xFF);
     //glColor(pr[i],pg[i],pb[i]);
     //glBegin(GL_POLYGON);
    //  for(j=0;j<NS;j++)
    //  {
    //     glTexCoord(clut[j],slut[j]);
    //     glVertex(clut[j]*r+x,slut[j]*r+y,-1);
    //  }
    //  glEnd();
     px[i] += pvx[i]; //*dtim;
     py[i] += pvy[i]; //*dtim;
     if (px[i] <       r) pvx[i] = fabs(pvx[i]);
     if (py[i] <       r) pvy[i] = fabs(pvy[i]);
     if (px[i] >= xres-r) pvx[i] =-fabs(pvx[i]);
     if (py[i] >= yres-r) pvy[i] =-fabs(pvy[i]);
  }  
  numframes++;
}


void redraw() {
  SDL_SetRenderDrawColor(renderer, /* RGBA: black */ 0x00, 0x00, 0x00, 0xFF);
  SDL_RenderClear(renderer);
  balls_to_the_walls();
  SDL_RenderPresent(renderer);
}

uint32_t ticksForNextKeyDown = 0;

bool handle_events() {
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT) {
    return false;
  }
  if (event.type == SDL_KEYDOWN) {
    uint32_t ticksNow = SDL_GetTicks();
    if (SDL_TICKS_PASSED(ticksNow, ticksForNextKeyDown)) {
      // Throttle keydown events for 10ms.
      ticksForNextKeyDown = ticksNow + 10;
      switch (event.key.keysym.sym) {
        case SDLK_UP:
          center.y -= 1;
          break;
        case SDLK_DOWN:
          center.y += 1;
          break;
        case SDLK_RIGHT:
          center.x += 1;
          break;
        case SDLK_LEFT:
          center.x -= 1;
          break;
      }
      
    }
  }
  redraw();
  return true;
}

void run_main_loop() {
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop([]() { handle_events(); }, 0, true);
#else
  while (handle_events())
    ;
#endif
}

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_CreateWindowAndRenderer(xres, yres, 0, &window, &renderer);

  redraw();
  run_main_loop();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
}