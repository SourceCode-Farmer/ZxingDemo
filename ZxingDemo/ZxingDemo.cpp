//按项目实际路径调整
#pragma comment(lib,"zxing_lib/ZXingCore.lib")

#include "stdafx.h"

#include "zxing_header\HybridBinarizer.h"
#include "zxing_header\LuminanceSource.h"
#include "zxing_header\GenericLuminanceSource.h"
#include "zxing_header\DecodeHints.h"
#include "zxing_header\BinaryBitmap.h"
#include "zxing_header\ReadBarcode.h"
#include "zxing_header\TextUtfEncoding.h"
#include "zxing_header\MultiFormatReader.h"

#include <opencv2\opencv.hpp>
#include <opencv2\core\types_c.h>


static std::string WstringToString(const std::wstring &wstr) {
	std::string str;
	std::mbstate_t state = {};
	const wchar_t *data = wstr.data();
	size_t len = std::wcsrtombs(nullptr, &data, 0, &state);
	if (static_cast<size_t>(-1) != len) {
		std::unique_ptr<char[]> buff(new char[len + 1]);
		len = std::wcsrtombs(buff.get(), &data, len, &state);
		if (static_cast<size_t>(-1) != len) {
			str.assign(buff.get(), len);
		}
	}
	return str;
}

static void Scan() {
	cv::Mat mat, gray_mat;
	//打开图片
	mat = cv::imread("test.PNG");
	if (mat.empty()) {
		std::cout << "not found image" << std::endl;
		return;
	}
	//转为灰度图
	cv::cvtColor(mat, gray_mat, cv::COLOR_RGBA2GRAY);
	//宽高
	int height = gray_mat.rows;
	int width = gray_mat.cols;
	auto *pixels = new unsigned char[height * width];
	int index = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pixels[index++] = gray_mat.at<unsigned char>(i, j);
		}
	}
	//识别
	std::shared_ptr<ZXing::GenericLuminanceSource> luminance = std::make_shared<ZXing::GenericLuminanceSource>(0, 0, width, height, pixels, width * sizeof(unsigned char));
	std::shared_ptr<ZXing::BinaryBitmap> bitmap = std::make_shared<ZXing::HybridBinarizer>(luminance);
	ZXing::DecodeHints hints;
	//根据需要添加format
	std::vector<ZXing::BarcodeFormat> formats = { ZXing::BarcodeFormat(ZXing::BarcodeFormat::QR_CODE) };
	hints.setPossibleFormats(formats);
	auto reader = new ZXing::MultiFormatReader(hints);
	ZXing::Result result = reader->read(*bitmap);
	if (result.status() == ZXing::DecodeStatus::NoError) {
		//识别成功，打印结果
		std::cout << WstringToString(result.text()) << std::endl;
	}
	else {
		std::cout << "Fail" << std::endl;
	}
}

int main()
{
	Scan();
	std::cin.get();
    return 0;
}

