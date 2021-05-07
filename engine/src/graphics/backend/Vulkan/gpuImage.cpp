#include "pch.h"
#include "gpuImage.h"
#if __has_include("gpuImage.g.cpp")
#include "gpuImage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::::implementation
{
    gpuImage::gpuImage()
    {
        InitializeComponent();
    }

    int32_t gpuImage::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void gpuImage::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void gpuImage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
    {
        Button().Content(box_value(L"Clicked"));
    }
}
