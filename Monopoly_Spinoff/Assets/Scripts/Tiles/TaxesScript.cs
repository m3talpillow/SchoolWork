//Matthew Drabek
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TaxesScript : TileScript
{
    public string descript;
    GameManagerScript gm;

    private void Start()
    {
        gm = GameManagerScript.instance();
    }



    /*             TILESCRIPT INHERITANCE                */

    // Tax either 200 or 10%.
    public override void Activate()
    {
        if (gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetCash() >= 200)
		{
            gm.GetCurrentPlayer().GetComponent<PlayerScript>().RemvCash(200);
            InfoScript.instance().Displayer("You've been taxed 200!");
        }
		else
		{
            int cash = gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetCash();

            gm.GetCurrentPlayer().GetComponent<PlayerScript>().RemvCash(cash/10);
            InfoScript.instance().Displayer("You've been taxed 10%!");
        }
    }

    // Displaying tile info
    public override string TileInfo()
    {
        return descript;
    }
}
