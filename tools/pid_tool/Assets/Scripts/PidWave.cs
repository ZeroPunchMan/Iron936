using System.Collections;
using System.Collections.Generic;
using System.Net;
using UnityEngine;
using UnityEngine.UI;

public class PidWave : MonoBehaviour
{
    RawImage ri = null;
    Texture2D t2d = null;
    const int waveWidth = 800;
    const int waveHeight = 200;
    int[] pidMeasure = new int[waveWidth];
    int[] pidTarget = new int[waveWidth];
    // Start is called before the first frame update
    void Start()
    {
        t2d = new Texture2D(waveWidth, waveHeight);
        for (int i = 0; i < t2d.width; i++)
        {
            for (int k = 0; k < t2d.height; k++)
            {
                t2d.SetPixel(i, k, Color.black);
            }
            pidMeasure[i] = 0;
            pidTarget[i] = 0;
        }
        t2d.Apply();

        ri = transform.GetComponent<RawImage>();
        ri.texture = t2d;
    }

    //string yourFile = "D:/computeTex.png"; //路径记得要修改！注意该路径斜线是向左的，和电脑里文件将爱路径相反
    //public void saveTexture2D(Texture2D texture, string file)
    //{
    //    byte[] bytes = texture.EncodeToPNG();
    //    System.IO.File.WriteAllBytes(file, bytes);
    //    Debug.Log("write to File over");
    //    UnityEditor.AssetDatabase.Refresh(); //自动刷新资源
    //}

    short testa = 0;
    // Update is called once per frame
    void Update()
    {
        AddNewStep(80, testa, 10);
        //UpdatePoint(80, testa);
        UpdateWave();

        testa++;
        if (testa >= waveHeight)
            testa = 0;
    }

    int headPoint = 0;
    void UpdatePoint(short target, short measure)
    {
        //清后面一段
        int pos = (headPoint + 50) % waveWidth;
        t2d.SetPixel(pos, pidMeasure[pos], Color.black);
        t2d.SetPixel(pos, pidTarget[pos], Color.black);

        //换算当前点坐标,并记录
        pidTarget[headPoint] = target;
        pidMeasure[headPoint] = measure;

        //绘制当前点坐标
        t2d.SetPixel(headPoint, target, new Color(0, 1, 0, 1));
        t2d.SetPixel(headPoint, measure, new Color(1, 0, 0, 1));

        //head右移
        headPoint++;
        headPoint %= waveWidth;

    }

    void UpdateWave()
    {
        t2d.Apply();
    }

    void AddNewStep(short target, short measure, int span)
    {
        int lastPoint;
        if (headPoint == 0)
            lastPoint = waveWidth - 1;
        else
            lastPoint = (headPoint - 1);

        float oMeasure = pidMeasure[lastPoint];
        float oTarget = pidTarget[lastPoint];
        for (int i = 1; i <= span; i++)
        {
            float ratio = (float)i / span;

            float tar = Mathf.Lerp(oTarget, target, ratio);
            float mea = Mathf.Lerp(oMeasure, measure, ratio);

            UpdatePoint((short)tar, (short)mea);
        }
    }

}
