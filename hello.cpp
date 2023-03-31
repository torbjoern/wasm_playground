#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <emscripten/val.h>

#include <string>
#include <stdio.h> // sprintf
#include <math.h>

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

void cls(emscripten::val &ctx)
{
    ctx.set("fillStyle", "black");
    ctx.call<void>("fillRect", 0, 0, xres, yres);
}
void circ(emscripten::val &ctx, double x, double y, double r)
{
  ctx.call<void>("beginPath");
  ctx.call<void>("arc", x, y, r, 0, 2 * PI);
  ctx.call<void>("fill");
}

static int numframes = 0;
void balls_to_the_walls(emscripten::val &ctx)
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
     sprintf(buf, "#%x%x%x", cr,cg,cb);

     ctx.set("fillStyle", std::string(buf) );
     circ(ctx, x, y, r);
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



auto main() -> int {
  const auto document = emscripten::val::global("document");
  const auto canvas =
      document.call<emscripten::val, std::string>("querySelector", "canvas");

  auto ctx = canvas.call<emscripten::val, std::string>("getContext", "2d");

  xres = canvas["width"].as<int>();
  yres = canvas["height"].as<int>();

  printf("canvas size:%d,%d\n", xres, yres);
  printf("wuta\n");
  char buf[256];
  sprintf(buf, "#%x%x%x", 2, 42, 64);
  printf("color:%s\n", buf );

  ctx.call<void>("clearRect", 0, 0, xres, yres);

  // rect
  ctx.set("fillStyle", "white");
  ctx.call<void>("fillRect", 0, 0, xres, yres);

  // rect
  ctx.set("fillStyle", "red");
  ctx.call<void>("fillRect", 10, 10, 40, 20);  

  // line
  ctx.set("strokeStyle", "black");
  ctx.call<void>("moveTo", 0, 0);
  ctx.call<void>("lineTo", xres, yres);
  ctx.call<void>("stroke");

  // text
  ctx.set("fillStyle", "black");
  ctx.set("font", "bold 48px serif");
  ctx.call<void, std::string>("fillText", "Hello World!", xres / 2,
                              yres / 2);

  int x = 0;
  int y = 0;
  int dx = 1;
  int dy = 1;
  while(1)
  {
    cls(ctx);
    balls_to_the_walls(ctx);
    ctx.set("fillStyle", "blue");
    circ(ctx, x, y, 10);
    x+=dx;
    y+=dy;
    if (x > xres) dx*=-1;  
    if (x < 0) dx*=-1;
    if (y > yres) dy*=-1;  
    if (y < 0) dy*=-1;
#ifdef __EMSCRIPTEN__
    emscripten_sleep(0);
#endif
  }

  return 0;
}