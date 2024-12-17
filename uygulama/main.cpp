#include <wx/wx.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/bitmap.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

private:
    // GUI Elements
    wxPanel* mainPanel;
    wxBoxSizer* mainSizer;
    wxBoxSizer* imageSizer;
    wxStaticBitmap* imagePanel1;
    wxStaticBitmap* imagePanel2;
    wxRadioBox* filterRadioBox;
    wxButton* changeButton;
    wxButton* applyButton;
    wxButton* saveButton;
    wxButton* selectImageButton;
    wxBitmap selectedImage;
     Mat originalImage;  // Orijinal resmi saklamak için
     Mat processedImage; // İşlenmiş resmi saklamak için
    wxStaticText* thresholdLabel;
    wxSlider* thresholdSlider;
    wxStaticText* kernelLabel;
    wxSlider* kernelSlider;
    wxStaticText* sigmaLabel;
    wxSlider* sigmaSlider;
    wxStaticText* BrightnessLabel;
    wxSlider* BrightnessSlider;
    wxStaticText* ContrastLabel;
    wxSlider* ContrastSlider;
    int thresholdValue;
    int kernelSize;
    int sigma;
    int brightness;
    float contrast;

    void UpdateImagePanel(wxCommandEvent& event, const  Mat& image);
    void OnThresholdChange(wxCommandEvent& event);
    void OnKernelChange(wxCommandEvent& event);
    void OnSigmaChange(wxCommandEvent& event);
    void OnBrigtnessChange(wxCommandEvent& event);
    void OnContrastChange(wxCommandEvent& event);
    void OnFilterSelectionChange(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnSelectImage(wxCommandEvent& event);
    void ApplyFilter(const wxString& filter);
    void DisplayImage(const  Mat& img, wxStaticBitmap* panel);

    // Filter functions
    void ApplyBGR2WhiteBlack( Mat& img, int thresholdVal);
    void ApplyBGR2Gray( Mat& img);
    void ApplyGaussianFilter( Mat& img, int kernelSize, double sigma);
    void ApplyBGR2HSV( Mat& img);
    void ApplyBGR2CMY( Mat& img);
    void ApplyNegative( Mat& img);
    void ApplyDilation( Mat& img, int kernelSize);
    void ApplyClosing( Mat& img, int kernelSize);
    void ApplyErosion( Mat& img, int kernelSize);
    void ApplyOpening( Mat& img, int kernelSize);
    void ApplyMedianFilter( Mat& img, int kernelSize);
    void ApplySobelFilter( Mat& image);
    void DrawHistogram( Mat& image);
    void BrightnessOperate( Mat& image);
    void ContrastOperate( Mat& image);
    void LabelingOperate( Mat& image);
};

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1000, 1000)) {

    mainPanel = new wxPanel(this, wxID_ANY);
    mainSizer = new wxBoxSizer(wxVERTICAL);
    mainPanel->SetSizer(mainSizer);
    imageSizer = new wxBoxSizer(wxHORIZONTAL);

    imagePanel1 = new wxStaticBitmap(mainPanel, wxID_ANY, wxBitmap(400, 400));
    imagePanel2 = new wxStaticBitmap(mainPanel, wxID_ANY, wxBitmap(400, 400));
    imageSizer->Add(imagePanel1, 0, wxALL, 10);
    imageSizer->Add(imagePanel2, 0, wxALL, 10);
    mainSizer->Add(imageSizer, 1, wxEXPAND);


    wxString filterChoices[] = {
        "Gauss", "Median","Sobel", "Açma", "Aşındırma", "Kapama", "Yayma",
        "BGR2CMY","Negative", "BGR2Gray", "BGR2HSV", "BGR2WhiteBlack",
        "Histogram Çiz", "Parlaklık Ayarla", "Kontrast Ayarla","Etiketle"
    };
    filterRadioBox = new wxRadioBox(mainPanel, wxID_ANY, "Filter Options", wxDefaultPosition, wxDefaultSize,WXSIZEOF(filterChoices), filterChoices, 1, wxRA_SPECIFY_COLS);
    imageSizer->Add(filterRadioBox, 0, wxALL | wxCENTER, 10);
    filterRadioBox->Bind(wxEVT_RADIOBOX, &MyFrame::OnFilterSelectionChange, this);

    //Brigtness Label
    BrightnessLabel = new wxStaticText(mainPanel, wxID_ANY, "Parlaklık");
    mainSizer->Add(BrightnessLabel, 0, wxALL | wxALIGN_CENTER, 5);
    BrightnessLabel->Hide();
    //Brigtness slider
    BrightnessSlider = new wxSlider(mainPanel, wxID_ANY, 0, -100, 100, wxDefaultPosition, wxSize(300, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    BrightnessSlider->Bind(wxEVT_SLIDER, &MyFrame::OnBrigtnessChange, this);
    mainSizer->Add(BrightnessSlider, 0, wxALL | wxALIGN_CENTER, 10);
    BrightnessSlider->Hide();
    
    //contasrt Label
    ContrastLabel = new wxStaticText(mainPanel, wxID_ANY, "Kontrast");
    mainSizer->Add(ContrastLabel, 0, wxALL | wxALIGN_CENTER, 5);
    ContrastLabel->Hide();
    //Contrast slider
    ContrastSlider = new wxSlider(mainPanel, wxID_ANY, 10, 5, 15, wxDefaultPosition, wxSize(300, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    ContrastSlider->Bind(wxEVT_SLIDER, &MyFrame::OnContrastChange, this);
    mainSizer->Add(ContrastSlider, 0, wxALL | wxALIGN_CENTER, 10);
    ContrastSlider->Hide();

    //resim seç
    selectImageButton = new wxButton(mainPanel, wxID_ANY, "Resim Seç");
    selectImageButton->Bind(wxEVT_BUTTON, &MyFrame::OnSelectImage, this);
    mainSizer->Add(selectImageButton, 0, wxALL | wxALIGN_CENTER, 10);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    //sola aktar
    changeButton = new wxButton(mainPanel, wxID_ANY, "Sola Aktar");
    changeButton->Bind(
        wxEVT_BUTTON,
        [this](wxCommandEvent& event) {
        UpdateImagePanel(event, processedImage);
        });
    buttonSizer->Add(changeButton, 0, wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

    //uygula
    applyButton = new wxButton(mainPanel, wxID_ANY, "Uygula");
    applyButton->Bind(wxEVT_BUTTON, &MyFrame::OnApply, this);
    buttonSizer->Add(applyButton, 0, wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

    //kaydet
    saveButton = new wxButton(mainPanel, wxID_ANY, "Kaydet");
    saveButton->Bind(wxEVT_BUTTON, &MyFrame::OnSave, this);
    buttonSizer->Add(saveButton, 0, wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

    //thereshold
    thresholdLabel = new wxStaticText(mainPanel, wxID_ANY, "Threshold Value:");
    mainSizer->Add(thresholdLabel, 0, wxALL | wxALIGN_CENTER, 5);
    thresholdLabel->Hide();
    //thereshold slider
    thresholdSlider = new wxSlider(mainPanel, wxID_ANY, 190, 0, 255,wxDefaultPosition, wxSize(300, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    thresholdSlider->Bind(wxEVT_SLIDER, &MyFrame::OnThresholdChange, this);
    mainSizer->Add(thresholdSlider, 0, wxALL | wxALIGN_CENTER, 10);
    thresholdSlider->Hide();

    //kernel
    kernelLabel = new wxStaticText(mainPanel, wxID_ANY, "Kernel Size:");
    mainSizer->Add(kernelLabel, 0, wxALL | wxALIGN_CENTER, 5);
    kernelLabel->Hide();
    //kernelSlider
    kernelSlider = new wxSlider(mainPanel, wxID_ANY, 3, 3, 9,wxDefaultPosition, wxSize(300, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    kernelSlider->Bind(wxEVT_SLIDER, &MyFrame::OnKernelChange, this);
    mainSizer->Add(kernelSlider, 0, wxALL | wxALIGN_CENTER, 10);
    kernelSlider->Hide();

    //sigma
    sigmaLabel = new wxStaticText(mainPanel, wxID_ANY, "sigma Value:");
    mainSizer->Add(sigmaLabel, 0, wxALL | wxALIGN_CENTER, 5);
    sigmaLabel->Hide();
    //sigmaSlider
    sigmaSlider = new wxSlider(mainPanel, wxID_ANY, 1, 1, 2,wxDefaultPosition, wxSize(300, -1), wxSL_HORIZONTAL | wxSL_LABELS);
    sigmaSlider->Bind(wxEVT_SLIDER, &MyFrame::OnSigmaChange, this);
    mainSizer->Add(sigmaSlider, 0, wxALL | wxALIGN_CENTER, 10);
    sigmaSlider->Hide();

    mainPanel->Layout();
}

void MyFrame::ApplyFilter(const wxString& filter) {
    processedImage = originalImage.clone();  // Reset to the original image

    if (filter == "BGR2WhiteBlack") {
        ApplyBGR2WhiteBlack(processedImage, thresholdValue);
    }
    else if (filter == "BGR2HSV") {
        ApplyBGR2HSV(processedImage);
    }
    else if (filter == "BGR2CMY") {
        ApplyBGR2CMY(processedImage);
    }
    else if (filter == "Negative") {
        ApplyNegative(processedImage);
    }
    else if (filter == "Yayma") {
        int kernelSize = 3;
        ApplyDilation(processedImage, kernelSize);
    }
    else if (filter == "Kapama") {
        int kernelSize = 3;
        ApplyClosing(processedImage, kernelSize);
    }
    else if (filter == "Aşındırma") {
        int kernelSize = 3;
        ApplyErosion(processedImage, kernelSize);
    }
    else if (filter == "Açma") {
        int kernelSize = 3;
        ApplyOpening(processedImage, kernelSize);
    }
    else if (filter == "Median") {
        ApplyMedianFilter(processedImage, kernelSize);
    }
    else if (filter == "Sobel") {
        ApplySobelFilter(processedImage);
    }
    else if (filter == "BGR2Gray") {
        ApplyBGR2Gray(processedImage);
    }
    else if (filter == "Gauss") {
        ApplyGaussianFilter(processedImage, kernelSize, sigma);
    }
    else if (filter == "Histogram Çiz") {
        DrawHistogram(processedImage);
    }
    else if (filter == "Parlaklık Ayarla") {
        BrightnessOperate(processedImage);
    }
    else if (filter == "Kontrast Ayarla") {
        ContrastOperate(processedImage);
    }
    else if (filter == "Etiketle") {
        LabelingOperate(processedImage);
    }

    // İşlem uygulandıktan sonra resmi ikinci panele göster
    DisplayImage(processedImage, imagePanel2);
}

void MyFrame::OnFilterSelectionChange(wxCommandEvent& event) {
    int selection = filterRadioBox->GetSelection();
    wxString selectedFilter = filterRadioBox->GetString(selection);

    if (selectedFilter == "BGR2WhiteBlack") {
        thresholdLabel->Show(); // Show threshold label
        thresholdSlider->Show(); // Show threshold slider
        kernelLabel->Hide(); // Hide kernel label
        kernelSlider->Hide(); // Hide kernel slider
        sigmaLabel->Hide(); // Hide sigma label
        sigmaSlider->Hide(); // Hide sigma slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
    }
    else if (selectedFilter == "Gauss") {

        kernelLabel->Show(); // Show kernel label
        kernelSlider->Show(); // Show kernel slider
        sigmaLabel->Show(); // Show sigma label
        sigmaSlider->Show(); // Show sigma slider
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
    }
    else if (selectedFilter == "Median") {

        kernelLabel->Show(); // Show kernel label
        kernelSlider->Show(); // Show kernel slider
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        sigmaLabel->Hide(); // Show sigma label
        sigmaSlider->Hide(); // Show sigma slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
    }
    else if (selectedFilter == "Yayma") {
        kernelLabel->Show(); // Show kernel label
        kernelSlider->Show(); // Show kernel slider
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        sigmaLabel->Hide(); // Show sigma label
        sigmaSlider->Hide(); // Show sigma slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
    }
    else if (selectedFilter == "Aşındırma") {
        kernelLabel->Show(); // Show kernel label
        kernelSlider->Show(); // Show kernel slider
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        sigmaLabel->Hide(); // Show sigma label
        sigmaSlider->Hide(); // Show sigma slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
    }
    else if (selectedFilter == "Parlaklık Ayarla") {
        BrightnessLabel->Show(); // Show brig label
        BrightnessSlider->Show(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        kernelLabel->Hide(); // Show kernel label
        kernelSlider->Hide(); // Show kernel slider
        sigmaLabel->Hide(); // Show sigma label
        sigmaSlider->Hide(); // Show sigma slider


    } 
    else if (selectedFilter == "Kontrast Ayarla") {

        ContrastLabel->Show(); // Show conr label
        ContrastSlider->Show(); // Show cont slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        kernelLabel->Hide(); // Show kernel label
        kernelSlider->Hide(); // Show kernel slider
        sigmaLabel->Hide(); // Show sigma label
        sigmaSlider->Hide(); // Show sigma slider


    }

    else {
        thresholdLabel->Hide(); // Hide threshold label
        thresholdSlider->Hide(); // Hide threshold slider
        kernelLabel->Hide(); // Hide kernel label
        kernelSlider->Hide(); // Hide kernel slider
        sigmaLabel->Hide(); // Hide sigma label
        sigmaSlider->Hide(); // Hide sigma slider
        BrightnessLabel->Hide(); // Show brig label
        BrightnessSlider->Hide(); // Show brig slider
        ContrastLabel->Hide(); // Show conr label
        ContrastSlider->Hide(); // Show cont slider
    }

    mainPanel->Layout(); // Düzeni yeniden oluştur
}

void MyFrame::UpdateImagePanel(wxCommandEvent& event,const  Mat& image) {
    if (image.empty()) {
        return;
    }

    image.copyTo(originalImage);

    // Görüntüyü sol panelde (imagePanel1) göster
    DisplayImage(originalImage, imagePanel1);
}

void MyFrame::OnApply(wxCommandEvent& event) {
    int selection = filterRadioBox->GetSelection();
    wxString selectedFilter = filterRadioBox->GetString(selection);

    // Uygulanan filtreyi uygula
    ApplyFilter(selectedFilter);
}

void MyFrame::OnSave(wxCommandEvent& event) {
    wxFileDialog saveFileDialog(this, "Save Image", "", "",
        "Image Files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (saveFileDialog.ShowModal() == wxID_OK) {
        wxString filePath = saveFileDialog.GetPath();
        if ( imwrite(filePath.ToStdString(), processedImage)) {
            wxMessageBox("Image saved successfully!", "Success", wxICON_INFORMATION | wxOK);
        }
        else {
            wxMessageBox("Failed to save the image.", "Error", wxICON_ERROR | wxOK);
        }
    }
}

void MyFrame::OnSelectImage(wxCommandEvent& event) {
    wxFileDialog openFileDialog(this, "Resim Seç", "", "",
        "Image Files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_OK) {
        wxString filePath = openFileDialog.GetPath();

        // OpenCV ile resmi yükleyin (renkli olarak)
        originalImage =  imread(filePath.ToStdString(),  IMREAD_COLOR);
         cvtColor(originalImage, originalImage,  COLOR_BGR2RGB);

        if (originalImage.empty()) {
            wxMessageBox("Resim yüklenemedi. Lütfen geçerli bir resim dosyası seçtiğinizden emin olun.",
                "Hata", wxICON_ERROR | wxOK);
            return;
        }

        // Resmi ekranda göster
        DisplayImage(originalImage, imagePanel1);
    }
}

void MyFrame::OnThresholdChange(wxCommandEvent& event) {
    wxSlider* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider) {
        thresholdValue = slider->GetValue();
    }
}

void MyFrame::OnKernelChange(wxCommandEvent& event) {
    wxSlider* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider) {
        kernelSize = slider->GetValue();
        if (kernelSize % 2 == 0) {
            kernelSize += 1;  // Çift sayıları tek yap
        }
     
    }
}

void MyFrame::OnSigmaChange(wxCommandEvent& event) {
    wxSlider* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider) {
        sigma = slider->GetValue();
    }
}

void MyFrame::OnBrigtnessChange(wxCommandEvent& event) {
    wxSlider* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider) {
        brightness = slider->GetValue();
    }
}

void MyFrame::OnContrastChange(wxCommandEvent& event) {
    wxSlider* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider) {
        int roundedValue = slider->GetValue();
        contrast = roundedValue / 10.0f;  // 5 -> 0.5, 10 -> 1.0, 15 -> 1.5
        
    }
}

void MyFrame::DisplayImage(const  Mat& img, wxStaticBitmap* panel) {
    // Resmin orijinal boyut oranını koruyarak yeniden boyutlandır
    int width = img.cols;
    int height = img.rows;

    float scaleX = 400.0f / width;
    float scaleY = 400.0f / height;
    float scale =  min(scaleX, scaleY);

    int newWidth = static_cast<int>(width * scale);
    int newHeight = static_cast<int>(height * scale);

     Mat resizedImg;
     resize(img, resizedImg,  Size(newWidth, newHeight));

    wxImage image(resizedImg.cols, resizedImg.rows, resizedImg.data, true);
    if (image.IsOk()) {
        wxBitmap bitmap(image);
        panel->SetBitmap(bitmap);
        panel->GetParent()->Layout();  // Paneli yeniden düzenle
    }
    else {
        wxMessageBox("Image conversion failed.", "Error", wxICON_ERROR | wxOK);
    }
}


// Filter Implementations
void MyFrame::ApplyBGR2WhiteBlack( Mat& img, int thresholdVal) {
    // Convert BGR to Gray
     cvtColor(img, img,  COLOR_BGR2GRAY);
    //eşikleme
     Mat binaryImage(img.rows, img.cols, CV_8UC1);
    for (int i = 0; i < binaryImage.rows; i++) {
        for (int j = 0; j < binaryImage.cols; j++) {
            uchar pixelValue = img.at<uchar>(i, j);
            binaryImage.at<uchar>(i, j) = (pixelValue > thresholdVal) ? 255 : 0;
        }
    }
     cvtColor(binaryImage, img,  COLOR_GRAY2BGR);
}

void MyFrame::ApplyBGR2Gray( Mat& img) {
     Mat grayImage(img.rows, img.cols, CV_8UC1);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
             Vec3b bgr = img.at< Vec3b>(i, j);
            uchar blue = bgr[0];
            uchar green = bgr[1];
            uchar red = bgr[2];

            uchar gray = static_cast<uchar>(0.299 * red + 0.587 * green + 0.114 * blue);
            grayImage.at<uchar>(i, j) = gray;
               
        }
    }
     cvtColor(grayImage, img,  COLOR_GRAY2BGR);
}

void MyFrame::ApplyGaussianFilter( Mat& img, int kernelSize, double sigma) {
    // Clone the original image to apply the filter and keep the original intact
     Mat filteredImage = img.clone();

    // Create Gaussian kernel
     Mat kernel(kernelSize, kernelSize, CV_32F); // float matrix
    int center = kernelSize / 2; // Kernel center
    double sum = 0.0; // Sum for normalization
    double pi = 3.141592653589793; // Pi constant

    // Calculate kernel values
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            int x = i - center; // x distance from the center
            int y = j - center; // y distance from the center
            kernel.at<float>(i, j) = (1 / (2 * pi * sigma * sigma)) * exp(-((x * x + y * y) / (2 * sigma * sigma)));
            sum += kernel.at<float>(i, j); // Accumulate for normalization
        }
    }

    // Normalize the kernel
    for (int i = 0; i < kernelSize; i++) {
        for (int j = 0; j < kernelSize; j++) {
            kernel.at<float>(i, j) /= sum; // Normalize kernel
        }
    }

    // Apply the kernel to the image
    for (int i = center; i < img.rows - center; i++) {  // Kenarlardaki pikselleri atlamak için i=center'den başlıyoruz
        for (int j = center; j < img.cols - center; j++) {  // Aynı şekilde j=center'den başlıyoruz
             Vec3f pixelValue =  Vec3f(0.0f, 0.0f, 0.0f); // Renkli piksel için Vec3f

            // Each color channel for kernel convolution
            for (int ki = -center; ki <= center; ki++) {
                for (int kj = -center; kj <= center; kj++) {
                    // Pikselin çevresindeki renkli değerler ve kernel çarpımı
                     Vec3b pixel = img.at< Vec3b>(i + ki, j + kj); // Renkli pikseli al
                    pixelValue[0] += pixel[0] * kernel.at<float>(ki + center, kj + center); // Blue kanalı
                    pixelValue[1] += pixel[1] * kernel.at<float>(ki + center, kj + center); // Green kanalı
                    pixelValue[2] += pixel[2] * kernel.at<float>(ki + center, kj + center); // Red kanalı
                }
            }

            // Saturate the values to be within 0-255 range
            filteredImage.at< Vec3b>(i, j) =  Vec3b(
                 saturate_cast<uchar>(pixelValue[0]),
                 saturate_cast<uchar>(pixelValue[1]),
                 saturate_cast<uchar>(pixelValue[2])
            );
        }
    }

    // Return the filtered image to the caller
    img = filteredImage;
}

void MyFrame::ApplyBGR2HSV( Mat& img) {
    for (int y = 0;y < img.rows;y++) {
        for (int x = 0; x < img.cols;x++) {
             Vec3b bgr = img.at< Vec3b>(y, x);
            double B = bgr[0] / 255.0;
            double G = bgr[1] / 255.0;
            double R = bgr[2] / 255.0;

            float maxVal =  max({ R,G,B });
            float minVal =  min({ R,G,B });
            float delta = maxVal - minVal;

            float H = 0, S = 0, V = maxVal;

            if (delta != 0) {
                if (maxVal == R) {
                    H=60*(fmod((G - B) / delta, 6));
                }
                else if (maxVal == G) {
                    H=60*((B - R) / delta + 2);
                }
                else if (maxVal == B) {
                    H=60*((R-G) / delta + 4);
                }
                if (H < 0) H += 360;

                S = delta / maxVal;

            }
            img.at< Vec3b>(y, x)[0] = static_cast<uchar>(H / 360.0 * 255);
            img.at< Vec3b>(y, x)[1] = static_cast<uchar>(S * 255);
            img.at< Vec3b>(y, x)[2] = static_cast<uchar>(V * 255);

        }
    }
     cvtColor(img, img,  COLOR_BGR2RGB);

}

void MyFrame::ApplyBGR2CMY( Mat& img) {
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            // BGR değerlerini al
             Vec3b bgr = img.at< Vec3b>(y, x);
            uchar B = bgr[0];  // Blue
            uchar G = bgr[1];  // Green
            uchar R = bgr[2];  // Red

            // Normalize et ve CMY'yi hesapla
            uchar C = 255 - R;  // Cyan = 1 - Red
            uchar M = 255 - G;  // Magenta = 1 - Green
            uchar Y = 255 - B;  // Yellow = 1 - Blue

            // Yeni CMY değerlerini at
            img.at< Vec3b>(y, x)[0] = C;  // Cyan
            img.at< Vec3b>(y, x)[1] = M;  // Magenta
            img.at< Vec3b>(y, x)[2] = Y;  // Yellow
        }
    }
}

void MyFrame::ApplyNegative( Mat& img) {
    for (int y = 0; y < img.rows; y++) {
        for (int x = 0; x < img.cols; x++) {
            // BGR değerlerini al
             Vec3b bgr = img.at< Vec3b>(y, x);
            uchar B = bgr[0];  // Blue
            uchar G = bgr[1];  // Green
            uchar R = bgr[2];  // Red

            // Normalize et ve CMY'yi hesapla
            uchar C = 255 - R;  // Cyan = 1 - Red
            uchar M = 255 - G;  // Magenta = 1 - Green
            uchar Y = 255 - B;  // Yellow = 1 - Blue

            // Yeni CMY değerlerini at
            img.at< Vec3b>(y, x)[0] = C;  // Cyan
            img.at< Vec3b>(y, x)[1] = M;  // Magenta
            img.at< Vec3b>(y, x)[2] = Y;  // Yellow
        }
    }
     cvtColor(img, img,  COLOR_BGR2RGB);
}

void MyFrame::ApplyDilation( Mat& img, int kernelSize) {

    if (img.empty() || kernelSize < 1) {
         cerr << "Giriş görüntüsü boş veya kernel boyutu geçersiz!" <<  endl;
        return;
    }
    int padding = kernelSize / 2;

    // Genişletme için padding ekleyin
     cvtColor(img, img,  COLOR_BGR2GRAY);
     Mat paddedImg;
     copyMakeBorder(img, paddedImg, padding, padding, padding, padding,  BORDER_REPLICATE);

    // Çıkış matrisi oluştur
     Mat result = img.clone();

    // Her piksel için dilasyon uygula
    for (int y = padding; y < img.rows-padding; y++) {
        for (int x = padding; x < img.cols-padding; x++) {
            uchar maxValue = 0;

            // Kernel altındaki maksimum pikseli bul
            for (int ky = -padding; ky <= padding; ky++) {
                for (int kx = -padding; kx <= padding; kx++) {
                    int py = y + ky + padding;
                    int px = x + kx + padding;
                    maxValue =  max(maxValue, paddedImg.at<uchar>(py, px));
                }
            }

            // Sonuç matrisine maksimum piksel değerini ata
            result.at<uchar>(y, x) = maxValue;
        }
    }

    // Sonuç matrisi orijinal görüntüye kopyala
    img = result;
     cvtColor(result, img,  COLOR_GRAY2BGR);

}

void MyFrame::ApplyErosion( Mat& img, int kernelSize) {

    if (img.empty() || kernelSize < 1) {
         cerr << "Giriş görüntüsü boş veya kernel boyutu geçersiz!" <<  endl;
        return;
    }

    int padding = kernelSize / 2;

    // Genişletme için padding ekleyin
     cvtColor(img, img,  COLOR_BGR2GRAY);
     Mat paddedImg;
     copyMakeBorder(img, paddedImg, padding, padding, padding, padding,  BORDER_REPLICATE);

    // Çıkış matrisi oluştur
     Mat result = img.clone();

    // Her piksel için erozyon uygula
    for (int y = padding; y < img.rows-padding; y++) {
        for (int x = padding; x < img.cols-padding; x++) {
            uchar minValue = 255;  // Başlangıçta max değerini alıyoruz

            // Kernel altındaki minimum pikseli bul
            for (int ky = -padding; ky <= padding; ky++) {
                for (int kx = -padding; kx <= padding; kx++) {
                    int py = y + ky + padding;
                    int px = x + kx + padding;
                    minValue =  min(minValue, paddedImg.at<uchar>(py, px));
                }
            }

            // Sonuç matrisine minimum piksel değerini ata
            result.at<uchar>(y, x) = minValue;
        }
    }

    // Sonuç matrisi orijinal görüntüye kopyala
    img = result;
     cvtColor(result, img,  COLOR_GRAY2BGR);
}

void MyFrame::ApplyClosing( Mat& img, int kernelSize) {
    if (img.empty() || kernelSize < 1) {
         cerr << "Giriş görüntüsü boş veya kernel boyutu geçersiz!" <<  endl;
        return;
    }
    // once dilatasyon işlemi uygula
    ApplyDilation(img, kernelSize);
    // sonra erozyon işlemi uygula
    ApplyErosion(img, kernelSize);


}

void MyFrame::ApplyOpening( Mat& img, int kernelSize) {
    if (img.empty() || kernelSize < 1) {
         cerr << "Giriş görüntüsü boş veya kernel boyutu geçersiz!" <<  endl;
        return;
    }

    // İlk olarak erozyon işlemi uygula
    ApplyErosion(img, kernelSize);

    // Sonra dilatasyon işlemi uygula
    ApplyDilation(img, kernelSize);
}

void MyFrame::ApplyMedianFilter( Mat& img, int kernelSize) {
    if (img.empty() || kernelSize < 1) {
         cerr << "Giriş görüntüsü boş, kernel boyutu geçersiz veya çift sayı!" <<  endl;
        return;
    }

    int padding = kernelSize / 2;

    // Çıkış matrisi oluştur
     Mat result = img.clone();

    // Her piksel için median filtresi uygula
    for (int y = padding; y < img.rows - padding; y++) {
        for (int x = padding; x < img.cols - padding; x++) {
            // Kernel bölgesindeki pikselleri saklayacak vektörler
             vector<int> bluePixels;
             vector<int> greenPixels;
             vector<int> redPixels;

            // Kernel bölgesindeki pikselleri al
            for (int ky = -padding; ky <= padding; ky++) {
                for (int kx = -padding; kx <= padding; kx++) {
                    int py = y + ky;
                    int px = x + kx;

                    // Pikselin BGR kanallarını al
                     Vec3b pixel = img.at< Vec3b>(py, px);
                    bluePixels.push_back(pixel[0]);  // Mavi kanal
                    greenPixels.push_back(pixel[1]); // Yeşil kanal
                    redPixels.push_back(pixel[2]);   // Kırmızı kanal
                }
            }

            // Her kanal için medyanı hesapla
             sort(bluePixels.begin(), bluePixels.end());
             sort(greenPixels.begin(), greenPixels.end());
             sort(redPixels.begin(), redPixels.end());

            // Kanal başına medyanı bul
            int blueMedian = bluePixels[bluePixels.size() / 2];
            int greenMedian = greenPixels[greenPixels.size() / 2];
            int redMedian = redPixels[redPixels.size() / 2];

            // Median değeri sonucu matrisine yaz (BGR)
            result.at< Vec3b>(y, x) =  Vec3b(blueMedian, greenMedian, redMedian);
        }
    }

    // Sonuç matrisini orijinal görüntüye kopyala
    img = result;

    // Gri tonlamadan tekrar renkli görüntüye dönüştür
}

void MyFrame::ApplySobelFilter( Mat& image) {
    Mat gray, blurred, edged;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blurred, Size(5, 5), 1);
    image = blurred;
    int kernelX[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int kernelY[3][3] = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };

    // Görüntü boyutuna göre gradyan matrislerini oluştur
     Mat grad_x =  Mat::zeros(image.size(), CV_16S);
     Mat grad_y =  Mat::zeros(image.size(), CV_16S);

    // Sobel operatörlerini uygulama
    for (int y = 1; y < image.rows - 1; y++) {
        for (int x = 1; x < image.cols - 1; x++) {
            int sumX = 0, sumY = 0;

            // Sobel kernel'larını görüntü pikselleri ile çarp
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sumX += kernelX[ky + 1][kx + 1] * image.at<uchar>(y + ky, x + kx);
                    sumY += kernelY[ky + 1][kx + 1] * image.at<uchar>(y + ky, x + kx);
                }
            }

            // Gradyan X ve Y değerlerini gradyan matrisine atama
            grad_x.at<short>(y, x) = sumX;
            grad_y.at<short>(y, x) = sumY;
        }
    }

    // Gradyan magnitüdünü hesapla
     Mat grad_mag =  Mat::zeros(image.size(), CV_32F);
    for (int y = 0; y < image.rows; y++) {
        for (int x = 0; x < image.cols; x++) {
            // Magnitüdü hesapla
            float magnitude = sqrt(pow(grad_x.at<short>(y, x), 2) + pow(grad_y.at<short>(y, x), 2));
            grad_mag.at<float>(y, x) = magnitude;
        }
    }

    // Magnitüdü normalize et
     Mat grad_mag_normalized;
     normalize(grad_mag, grad_mag_normalized, 0, 255,  NORM_MINMAX);

    // Sonuçları 8 bit formatına dönüştür
    grad_mag_normalized.convertTo(image, CV_8U);

     cvtColor(image, image,  COLOR_GRAY2BGR);
}

void MyFrame::DrawHistogram( Mat& image)
{
    int histSize = 256;
    vector<float> histManual(histSize, 0);

    // Piksel değerlerini topla
    for (int i = 0; i < image.rows; i++) {
        for (int j = 0; j < image.cols; j++) {
            int pixelValue = image.at<uchar>(i, j); // Gri tonlamalı değeri al
            histManual[pixelValue]++;
        }
    }

    // Görselleştirme
    int hist_w = 512;    // Histogram genişliği
    int hist_h = 400;    // Histogram yüksekliği
    Mat histImage(hist_h + 100, hist_w + 100, CV_8UC3, Scalar(255, 255, 255)); // Beyaz arkaplan

    int maxVal = *max_element(histManual.begin(), histManual.end()); // En yüksek frekansı al

    int bin_w = cvRound((double)hist_w / histSize); // Her bin'in genişliği
    // Histogramı normalize et (yükseklik ayarı için)
    vector<int> normalizedHist(histSize);
    for (int i = 0; i < histSize; i++) {
        normalizedHist[i] = cvRound((double)histManual[i] / maxVal * hist_h);
    }

    // Dikey çubukları çiz ve yazıları ekle
    for (int i = 0; i < histSize; i++) {
        // Çubukları çiz
        rectangle(histImage,
            Point(bin_w * i + 50, hist_h - normalizedHist[i] + 50), // Sol üst
            Point(bin_w * (i + 1) + 50, hist_h + 50),               // Sağ alt
            Scalar(0, 0, 0),                                       // Çubuk rengi (siyah)
            FILLED);

        // Frekansı üstüne yaz
        string freqText = to_string(histManual[i]);


        // Bin ismini altına yaz
        string binText = "Bin " + to_string(i);
        putText(histImage, binText, Point(bin_w * i + 55, hist_h + 70),
            FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);
    }

    // Y eksenine çizgiler ve değerler ekle
    for (int y = 0; y <= 5; y++) { // 5 bölme yapıyoruz
        int yPos = hist_h - (y * hist_h / 5) + 50;
        line(histImage, Point(50, yPos), Point(hist_w + 50, yPos), Scalar(200, 200, 200), 1); // Yatay çizgi
        putText(histImage, to_string(y * maxVal / 5), Point(10, yPos + 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1); // Değer yazısı
    }

    image = histImage;

}

void MyFrame::BrightnessOperate( Mat& image) {
    processedImage = image.clone();
    for (int y = 0; y < processedImage.rows; y++) {
        for (int x = 0; x < processedImage.cols; x++) {
            for (int c = 0; c < processedImage.channels(); c++) {
                processedImage.at< Vec3b>(y, x)[c] =  saturate_cast<uchar>(image.at< Vec3b>(y, x)[c] + brightness);
            }
        }
    }
}

void MyFrame::ContrastOperate( Mat& image) {

    processedImage = image.clone();
    for (int y = 0; y < processedImage.rows; y++) {
        for (int x = 0; x < processedImage.cols; x++) {
            for (int c = 0; c < processedImage.channels(); c++) {
                uchar outputPixel =  saturate_cast<uchar>(contrast * (processedImage.at< Vec3b>(y, x)[c] - 128) + 128);
                processedImage.at< Vec3b>(y, x)[c] = outputPixel;
            }
        }
    }
}

void MyFrame::LabelingOperate( Mat& image) {

    Mat binaryImage;
     cvtColor(image, binaryImage,  COLOR_BGR2GRAY);
    threshold(binaryImage, binaryImage, 128, 255, THRESH_BINARY);

     Mat labeledImage =  Mat::zeros(binaryImage.size(), CV_32S);
    int currentLabel = 1; // İlk etiket

    int dx[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    int dy[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };

    // Etiketleme işlemi
    for (int y = 0; y < binaryImage.rows; y++) {
        for (int x = 0; x < binaryImage.cols; x++) {
            // Eğer piksel beyaz (255) ve henüz etiketlenmemişse
            if (binaryImage.at<uchar>(y, x) == 255 && labeledImage.at<int>(y, x) == 0) {
                // Yeni bir bağlı bileşen için BFS
                queue<Point> q;
                q.push( Point(x, y));
                labeledImage.at<int>(y, x) = currentLabel;

                while (!q.empty()) {
                     Point p = q.front();
                    q.pop();

                    for (int k = 0; k < 8; k++) {
                        int nx = p.x + dx[k];
                        int ny = p.y + dy[k];

                        if (nx >= 0 && nx < binaryImage.cols && ny >= 0 && ny < binaryImage.rows) {
                            if (binaryImage.at<uchar>(ny, nx) == 255 && labeledImage.at<int>(ny, nx) == 0) {
                                labeledImage.at<int>(ny, nx) = currentLabel;
                                q.push( Point(nx, ny));
                            }
                        }
                    }
                }

                currentLabel++; // Bir sonraki bağlı bileşen için yeni etiket
            }
        }
    }

    // Renkli görüntü oluştur
     Mat colorLabeledImage =  Mat::zeros(labeledImage.size(), CV_8UC3);
     map<int,  Vec3b> labelColors; // Etiket-renk eşleşmesi için harita

    for (int y = 0; y < labeledImage.rows; y++) {
        for (int x = 0; x < labeledImage.cols; x++) {
            int label = labeledImage.at<int>(y, x);
            if (label > 0) {
                // Eğer etiket için renk atanmadıysa yeni bir renk oluştur
                if (labelColors.find(label) == labelColors.end()) {
                    labelColors[label] =  Vec3b(rand() % 256, rand() % 256, rand() % 256);
                }
                colorLabeledImage.at< Vec3b>(y, x) = labelColors[label];
            }
        }
    }

    // Orijinal görüntüyü renklendirilmiş görüntü ile değiştir
    image = colorLabeledImage.clone();

}



class MyApp : public wxApp {
public:
    virtual bool OnInit();
};

bool MyApp::OnInit() {
    MyFrame* frame = new MyFrame("Image Processing Filters");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);
