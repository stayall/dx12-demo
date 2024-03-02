#include "Image.h"

using namespace std;

namespace stay
{
    ostream& operator<<(ostream& out, const Image& image)
    {
        out << "Image" << endl;
        out << "-----" << endl;
        out << "Width: " << image.Width << endl;
        out << "Height: " << image.Height << endl;
        out << "Bit Count: " << image.bitcount << endl;
        out << "Pitch: " << image.pitch << endl;
        out << "Data Size: " << image.data_size << endl;


        return out;
    }
}
