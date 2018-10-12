using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CardTile : TileScript
{
    public string descript;
    private GameManagerScript gm;

    void Start()
    {
        gm = GameManagerScript.instance();
    }



    /*             TILESCRIPT INHERITENCE           */

    // Pull a card
    public override void Activate()
    {
        gm.GetCard().GetComponent<CardScript>().Activate();
    }

    // Displaying tile info
    public override string TileInfo()
    {
        return descript;
    }

}
