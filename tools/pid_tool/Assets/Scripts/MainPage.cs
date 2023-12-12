using System;
using System.Collections;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using UnityEditor.Experimental.GraphView;
using UnityEngine;
using UnityEngine.UI;

public class MainPage : MonoBehaviour
{
    [SerializeField]
    Button btnClose = null;
    [SerializeField]
    Button btnOpenPort = null;
    [SerializeField]
    Button btnRefresh = null;
    [SerializeField]
    Button btnClearLog = null;
    [SerializeField]
    Dropdown ddPortList = null;
    [SerializeField]
    InputField inputBaudrate = null;
    [SerializeField]
    Text textLog = null;

    SerialPort serialPort = null;
    //ConcurrentQueue<byte[]> recvQueue = new ConcurrentQueue<byte[]>();


    bool portOpened
    {
        get
        {
            bool res = serialPort != null && serialPort.IsOpen;
            return res;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        btnClose.onClick.AddListener(this.OnCloseClick);
        btnOpenPort.onClick.AddListener(this.OnOpenPortClick);
        btnRefresh.onClick.AddListener(this.OnRefreshClick);
        btnClearLog.onClick.AddListener(this.OnClearLogClick);

        this.OnRefreshClick();
    }

    // Update is called once per frame
    void Update()
    {
        if (serialPort != null && !serialPort.IsOpen)
        {
            btnOpenPort.transform.Find("Text").GetComponent<Text>().text = "�򿪴���";
            ClosePort();
        }

        if (portOpened)
        {
            if (serialPort.BytesToRead > 0)
            {
                byte[] data = new byte[serialPort.BytesToRead];
                serialPort.Read(data, 0, data.Length);
                StringBuilder sb = new StringBuilder();
                foreach (byte b in data)
                {
                    sb.AppendFormat("{0:x2} ", b);
                }
                Debug.Log(sb.ToString());
            }
        }
    }

    void OnCloseClick()
    {
        Debug.Log("�رճ���");
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
    }
    bool TryOpenPort(string port, int baud)
    {
        serialPort = new SerialPort();
        //���ö˿�
        serialPort.PortName = port;
        //���ò�����
        serialPort.BaudRate = baud;
        //����У��
        serialPort.Parity = Parity.None;
        //��������λ��
        serialPort.DataBits = 8;
        //����ֹͣλ
        serialPort.StopBits = StopBits.One;
        serialPort.ReadTimeout = 0;

        //�������ݻص�
        //serialPort.DataReceived += this.OnReceivedData;
        //�ص���ֵ����Ϊ1
        //serialPort.ReceivedBytesThreshold = 1;

        try
        {
            serialPort.Open();
        }
        catch
        {
            serialPort = null;
            return false;
        }

        return true;
    }

    void ClosePort()
    {
        if (serialPort != null)
        {
            serialPort.Close();
            serialPort = null;
        }
    }

    //void OnReceivedData(object sender, SerialDataReceivedEventArgs e)
    //{
    //    byte[] data = new byte[serialPort.BytesToRead];
    //    serialPort.Read(data, 0, data.Length);//��ȡ����

    //    recvQueue.Enqueue(data);
    //    //todo
    //    //this.BeginInvoke(new AppendNewReceivedData(AppendRecvData), data);
    //}

    void OnOpenPortClick()
    {
        //Debug.Log("�򿪴���");
        if (portOpened)
        { //�رմ���
            ClosePort();
            btnOpenPort.transform.Find("Text").GetComponent<Text>().text = "�򿪴���";
            PrintLog("�رմ���");
        }
        else
        { //�򿪴���
            string port = null;
            int baud = 0;
            try
            {
                port = ddPortList.options[ddPortList.value].text;
                baud = int.Parse(inputBaudrate.text);
            }
            catch
            {
                PrintLog("���ڻ�������Ч");
            }

            if (TryOpenPort(port, baud))
            {
                btnOpenPort.transform.Find("Text").GetComponent<Text>().text = "�رմ���";
                PrintLog(string.Format("�򿪴��ڳɹ�: {0}, {1}", port, baud));
            }
            else
            {
                PrintLog("�򿪴���ʧ��");
            }
        }
    }

    void OnRefreshClick()
    {
        string[] strPorts = SerialPort.GetPortNames();
        List<string> listPorts = new List<string>(strPorts);

        ddPortList.ClearOptions();
        ddPortList.AddOptions(listPorts);
    }
    void OnClearLogClick()
    {
        textLog.text = null;
    }

    void OnDisable()
    {
        ClosePort();
    }

    void PrintLog(string s)
    {
        textLog.text += string.Format("{0}\n", s);
    }

}
