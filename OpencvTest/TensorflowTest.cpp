// OpencvTest.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//

#include <iostream>
#include<string>
#include<fstream>
#include<cmath>
#include <windows.h>
#include <time.h>

#include <Tlhelp32.h>
#include<opencv.hpp>
#include<imgproc.hpp>

#include "tensorflow/cc/ops/const_op.h"
#include "tensorflow/cc/ops/image_ops.h"
#include "tensorflow/cc/ops/standard_ops.h"
#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/default_device.h"
#include "tensorflow/core/graph/graph_def_builder.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/stringpiece.h"
#include "tensorflow/core/lib/core/threadpool.h"
#include "tensorflow/core/lib/io/path.h"
#include "tensorflow/core/lib/strings/stringprintf.h"
#include "tensorflow/core/platform/env.h"
#include "tensorflow/core/platform/init_main.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"
#include "tensorflow/core/util/command_line_flags.h"

#include "tensorflow/cc/client/client_session.h"
#include "tensorflow/cc/framework/gradients.h"


using namespace tensorflow;
using namespace tensorflow::ops;
using tensorflow::Flag;
using tensorflow::Tensor;
using tensorflow::Status;
using tensorflow::string;
using tensorflow::int32;


using namespace cv;
using namespace std;


/**
https://blog.csdn.net/yx123919804/article/details/107042822
https://blog.csdn.net/dycljj/article/details/118408400
https://www.jianshu.com/p/42589012c6f1
**/
void Mat_to_Tensor(Mat img, Tensor* output_tensor, int input_rows, int input_cols)
{
    //ͼ�����resize����
    resize(img, img, cv::Size(input_cols, input_rows));
    //imshow("resized image",img);

    //��һ��
    //img.convertTo(img,CV_32FC1);
    //img=1-img/255;

    //����һ��ָ��tensor�����ݵ�ָ��
    float* p = output_tensor->flat<float>().data();

    cv::Mat tempMat(input_rows, input_cols, CV_32FC3, p);   //ע��ת����ͼ��Ϊ��ɫͼ���ǻҶ�ͼ
    img.convertTo(tempMat, CV_32FC3);
}

int Tensor_to_Mat(const tensorflow::Tensor& inputTensor, cv::Mat& output)
{
    tensorflow::TensorShape inputTensorShape = inputTensor.shape();
    if (inputTensorShape.dims() != 4)
    {
        return -1;
    }

    int height = inputTensorShape.dim_size(1);
    int width = inputTensorShape.dim_size(2);
    int depth = inputTensorShape.dim_size(3);

    output = cv::Mat(height, width, CV_32FC(depth));
    auto inputTensorMapped = inputTensor.tensor<float, 4>();
    float* data = (float*)output.data;
    for (int y = 0; y < height; ++y)
    {
        float* dataRow = data + (y * width * depth);
        for (int x = 0; x < width; ++x)
        {
            float* dataPixel = dataRow + (x * depth);
            for (int c = 0; c < depth; ++c)
            {
                float* dataValue = dataPixel + c;
                *dataValue = inputTensorMapped(0, y, x, c);
            }
        }
    }
    return 0;
}



int main()
{
    HDC m_screenDC;
    HDC m_memDC;
    HBITMAP m_hBitmap;

    int width = 320, height = 320;

    // ��ȡ��Ļ DC
    m_screenDC = GetDC(HWND_DESKTOP);
    m_memDC = CreateCompatibleDC(m_screenDC);
    // ����λͼ
    m_hBitmap = CreateCompatibleBitmap(m_screenDC, width, height);
    SelectObject(m_memDC, m_hBitmap);


    cv::Mat mat, mat2;
    mat.create(height, width, CV_8UC4);
    //mat.create(height, width, CV_32FC4);

    //std::cout << "mat.type() = " << mat.type() << std::endl;
    //std::cout << "mat2.type() = " << mat2.type() << std::endl;

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);

    //cv::imshow("opencvwindows", mat);
    //cout << "show empty mat\n";
    //waitKey(2000);




    double duration1, duration2;
    clock_t start1, finish1, start2, finish2;

    start1 = clock();


    // �õ�λͼ������
    // ʹ��BitBlt��ͼ�����ܽϵͣ������޸�ΪDXGI
    //Windows8�Ժ�΢��������һ���µĽӿڣ��С�Desktop Duplication API����Ӧ�ó��򣬿���ͨ������API�����������ݡ�
    //����Desktop Duplication API��ͨ��Microsoft DirectX Graphics Infrastructure (DXGI)���ṩ����ͼ��ģ��ٶȷǳ��졣
    bool opt = BitBlt(m_memDC, 0, 0, width, height, m_screenDC, 300, 250, SRCCOPY);

    int iBits = GetDeviceCaps(m_memDC, BITSPIXEL) * GetDeviceCaps(m_memDC, PLANES);
    WORD wBitCount;
    if (iBits <= 1)
        wBitCount = 1;
    else if (iBits <= 4)
        wBitCount = 4;
    else if (iBits <= 8)
        wBitCount = 8;
    else if (iBits <= 24)
        wBitCount = 24;
    else
        wBitCount = 32;
    BITMAPINFOHEADER bi = { sizeof(bi), width, -height, 1, wBitCount, BI_RGB };


    //int rows = GetDIBits(m_screenDC, m_hBitmap, 0, detectRect.height, m_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    int rows = GetDIBits(m_memDC, m_hBitmap, 0, height, mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    //cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    //cv::imshow("opencvwindows", mat);
    cout << "show GetDIBits mat\n";
    //waitKey(10000);

    // ����mat����
    //ע�����˳��Ϊ �С��С�ͼ���ʽ��bitmap����
    //Mat screenshot = Mat(detectRect.height, detectRect.width, CV_8UC4, m_screenshotData);
    //Mat screenshot(detectRect.width, detectRect.height, CV_8UC4, m_screenshotData);
    //Mat screenshot(detectRect.height, detectRect.width, CV_32FC4, m_screenshotData);

    //��Ļ��ͼ����Ƶ��ͼ��ʽ��һ������Ҫ����ͼ���ʽת��
    //ȥ��alpha ֵ(͸����)ͨ����ת��ΪCV_8UC3��ʽ
    cv::cvtColor(mat, mat2, COLOR_RGBA2RGB);

    finish1 = clock();
    duration1 = (double)(finish1 - start1) * 1000 / CLOCKS_PER_SEC;

    //std::cout << "mat.type() = " << mat.type() << std::endl;
    //std::cout << "mat2.type() = " << mat2.type() << std::endl;

    //���ͼ���ʽ
    //std::cout << "mat��ʽ��" << mat.depth() << "," << mat.type() << "," << mat.channels() << "," << std::endl;
    //std::cout << "mat2��ʽ��" << mat2.depth() << "," << mat2.type() << "," << mat2.channels() << "," << std::endl;

    cv::imshow("opencvwindows", mat2);
    cout << "show cvtColor mat\n";
    waitKey(10000);




    /**
    const string ConfigFile = "../../Data/model/efficientdet/d0.pbtxt";
    const string ModelFile = "../../Data/model/efficientdet/d0.pb";
    const string LabelFile = "../../Data/model/efficientdet/coco.names";
    vector<string> m_classLabels; //���ǩ
    const float MinConfidence = 0.7; //��С���Ŷ�
    const int PersonClassId = 0; //�����ǩ�б�����Ա��λ��
    vector<String> m_outputsNames; //ģ������������
   */


   //const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3.pbtxt";
   //const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3.pb";
    const string ConfigFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pbtxt";
    const string ModelFile = "../../Data/model/mobilenet/ssd_mobilenet_v3_small.pb";
    const string LabelFile = "../../Data/model/mobilenet/coco.names";
    vector<string> m_classLabels; //���ǩ
    const float MinConfidence = 0.7; //��С���Ŷ�
    const int PersonClassId = 0; //�����ǩ�б�����Ա��λ��
    //cv::dnn::DetectionModel m_net;
    vector<String> m_outputsNames; //ģ������������



    //get the names of all the layers in the network
    //vector<String> layersNames = m_net.getLayerNames();

    //m_net.setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    //m_net.setPreferableTarget(dnn::DNN_TARGET_CPU);

    // ���ط����ǩ
    ifstream fin(LabelFile);
    if (fin.is_open())
    {
        string className = "";
        while (std::getline(fin, className))
            m_classLabels.push_back(className);
    }




    //����tensorflow�Ự
    Session* session;
    cout << "start initalize session" << "\n";
    Status status = tensorflow::NewSession(SessionOptions(), &session);
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }

    GraphDef graph_def;
    status = ReadBinaryProto(Env::Default(), ModelFile, &graph_def);
    //MNIST_MODEL_PATHΪģ�͵�·������model_frozen.pb��·��
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }
    for (int i = 0; i < graph_def.node_size(); i++)
    {
        std::string name = graph_def.node(i).name();
        std::cout << name << std::endl;
    }

    status = session->Create(graph_def);
    if (!status.ok()) {
        cout << status.ToString() << "\n";
        return 1;
    }
    cout << "tensorflowģ�ͼ��سɹ�" << "\n";


    //ת��ͼƬ
    int input_height = 320;
    int input_width = 320;
    Tensor input_tensor1(DT_FLOAT, TensorShape({ 1,input_height,input_width,3 }));
    //��Opencv��Mat��ʽ��ͼƬ����tensor
    Mat_to_Tensor(mat2, &input_tensor1, input_height, input_width);
    cout << input_tensor1.DebugString() << endl;
    


    //ǰ�����У�������һ����һ��tensor��vector
    vector<tensorflow::Tensor> outputs;
    string input_tensor_name = "normalized_input_image_tensor";
    string output_node = "detection_out";
    //cout << "Session Running......" << endl;
    Status status_run = session->Run({ { input_tensor_name, input_tensor1 } }, { output_node }, {}, &outputs);
    //cout << "Session complet......" << endl;
    if (!status_run.ok()) {
        cout << "ERROR: RUN failed..." << std::endl;
        cout << status_run.ToString() << "\n";
        exit(-1);
    }
    session->Close();



    Mat show_image;
    //vector<tensorflow::Tensor> outputs;
    for (int i = 0; i < outputs.size(); ++i)
    {
        cout << outputs[i].DebugString() << endl;

        Tensor_to_Mat(outputs[i], show_image);

    }

    cv::imshow("opencvwindows", show_image);
    cout << "show cvtColor mat\n";
    waitKey(10000);


    /*
    string times = format("%.2f, %.2f", duration1, duration2);
    putText(mat2, times, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);

    for (int i = 0; i < classIds.size(); i++) {
        rectangle(mat2, boxes[i], Scalar(255, 178, 50), 3);

        //Get the label for the class name and its confidence
        string label = format("%.2f", confidences[i]);
        if (!m_classLabels.empty())
        {
            //label = m_classLabels[classIds[i]-1] + ":" + label;
            label = to_string(classIds[i]) + "," + label;
        }

        //Display the label at the top of the bounding box
        int baseLine;
        Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        int top = max(boxes[i].y, labelSize.height);
        putText(mat2, label, Point(boxes[i].x, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 1);
    }
    */

    cv::namedWindow("opencvwindows", WINDOW_AUTOSIZE);
    cv::imshow("opencvwindows", mat2);


    waitKey(0);

    //cv::destroyAllWindows();


}

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ����ʹ�ü���: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
