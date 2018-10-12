//Matthew Drabek
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GOScript : TileScript
{
    public string descript;
	public int goValue = 200;

    public int GetGoValue()
    {
        return goValue;
    }

    /*             TILESCRIPT INHERITANCE                */

    public override void Activate()
    {
        InfoScript.instance().Displayer("You're on GO!");
    }

    // Displaying tile info
    public override string TileInfo()
    {
        return descript;
    }
}
