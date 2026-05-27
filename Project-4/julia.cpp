
#include <complex>
#include <fstream>

//----------------------------------------------------------------------------
//
//  Global configuration parameters
//

const size_t Width = 1024;
const size_t Height = 1024;
const size_t MaxIterations = 1000;
using Complex = std::complex<float>;

Complex operator * (Complex::value_type s, const Complex& z)
    { return Complex(s * z.real(), s * z.imag()); }

struct Color {
    using T = unsigned char;
    T r = 0;
    T g = 0;
    T b = 0;

    Color() = default;
    Color(T r, T g, T b) : r(r), g(g), b(b) {}
};

//----------------------------------------------------------------------------
//
//  function setColor()
//
//  This function returns a color based on an iteration value.
//
//  As this function is called from an executing GPU kernel (julia), it's
//    tagged as being a device-only function (using the __device__ decoration)
//

Color setColor(int iterations) {
    constexpr size_t NumColors = 16;
    const Color colors[NumColors] = {
        Color(66, 30, 15),
        Color(25, 7, 26),
        Color(9, 1, 47),
        Color(4, 4, 73),
        Color(0, 7, 100),
        Color(12, 44, 138),
        Color(24, 82, 177),
        Color(57, 125, 209),
        Color(134, 181, 229),
        Color(211, 236, 248),
        Color(241, 233, 191),
        Color(248, 201, 95),
        Color(255, 170, 0),
        Color(204, 128, 0),
        Color(153, 87, 0),
        Color(106, 52, 3)
    };

    const Color black;

    return iterations < MaxIterations ? colors[iterations % NumColors] : black;
}

//----------------------------------------------------------------------------
//
//  function main()
//
//  Nothing particularly special here.  We specify the region in the Complex
//    plane we're interested in looking at using its lower-left, and upper-
//    right corners (the variables "ll", and "ur", respectively).
//
//  From there, we determine the size of the region, its center point, and
//    the size of a pixel in the complex plane.
//
//  Once the computation is done, we output the results in an image named 
//    "julia.ppm".
//

inline float magnitude(const Complex& z) { return std::abs(z); }

int main() {
    Complex ll(-2.1, -2.1);
    Complex ur( 2.1,  2.1);
    Complex domain = ur - ll;
    Complex center = 0.5 * domain;
    Complex d(domain.real()/Width, domain.imag()/Height);

    Color pixels[Height * Width];

    for (auto y = 0; y < Height; ++y) {
        for (auto x = 0; x < Width; ++x) {
            Complex c(x*d.real(), y*d.imag());
            c -= center;
            Complex z;
            
            int iter = 0;
            while (iter < MaxIterations && magnitude(z) < 2.0) {
                z = z*z + c;
                ++iter;
            }
            
            pixels[x + y * Width] = setColor(iter);
        }
    }

    std::ofstream ppm("julia.ppm", std::ios::binary);
    ppm << "P6 " << Width << " " << Height << " " << 255 << "\n";
    ppm.write(reinterpret_cast<const char*>(&pixels[0]), sizeof(pixels));
}
