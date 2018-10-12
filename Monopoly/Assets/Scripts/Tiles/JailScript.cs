using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class JailScript : TileScript
{
    public string descript;
    private int escapeRollNum;
    // need to set bail price
    private int bailPrice;


    // get bail price
    public int GetBailPrice()
    {
        return bailPrice;
    }

    // Increments how many turns the player has been in jail. Game manager will need to determine
    // if the player needs to force the player to pay the bail after three rolls
    public int IncrementEscapeRollNum()
    {
        escapeRollNum++;
        return escapeRollNum;
    }



    /*             TILESCRIPT INHERITANCE                */

    // standard activation method
    public override void Activate()
    {
        InfoScript.instance().Displayer("Just visiting jail");
    }

    // Displaying tile info
    public override string TileInfo()
    {
        return descript;
    }
}
