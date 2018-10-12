using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class TileButtonScript : MonoBehaviour
{
    private int tileNum;
    private Text t;
    private GameManagerScript gm;

    void Start()
    {
        gm = GameManagerScript.instance();
    }

    public void SetTileNum(int num)
    {
        tileNum = num;
    }

    public int GetTileNum()
    {
        return tileNum;
    }

    public void SetTextObject(Text t)
    {
        this.t = t;
    }

    public void WriteTileToConsole()
    {
        // update upgrade, sell, and mortgage buttons
        gm.SetUpgradeScript(tileNum);

        // Get tile info should come from
        GameObject tile = gm.GetComponent<GameManagerScript>().GetTile(tileNum);
        t.GetComponent<Text>().text = "Console > " + tile.GetComponent<TileScript>().GetName() + "\n\n";

        t.GetComponent<Text>().text += tile.GetComponent<TileScript>().TileInfo();
    }
}
