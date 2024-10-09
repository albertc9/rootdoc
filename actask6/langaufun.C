
double langaufun(double *x, double *par) {

   // Fit parameters:
   // par[0]=Width (scale) parameter of Landau density
   // par[1]=Most Probable (MP, location) parameter of Landau density
   // par[2]=Total area (integral -inf to inf, normalization constant)
   // par[3]=Width (sigma) of convoluted Gaussian function

   // Numeric constants
   double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
   double mpshift  = -0.22278298;       // Landau maximum location 应该是Landau分布峰值位置的修正

   // Control constants
   double np = 100.0;      // number of convolution steps
   double sc = 5.0;      // convolution extends to +-sc Gaussian sigmas

   // Variables
    double xx;
    double mpc;
    double fland;
    double sum = 0.0;
    double xlow,xupp;
    double step;
    double i;

   // MP shift correction
   mpc = par[1] - mpshift * par[0];

   // Range of convolution integral
   xlow = x[0] - sc * par[3];
   xupp = x[0] + sc * par[3];

    step = (xupp-xlow) / np;

   // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);

      xx = xupp - (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }

   return (par[2] * step * sum * invsq2pi / par[3]);
}