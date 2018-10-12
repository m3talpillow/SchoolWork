using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GoToJailScript : TileScript
{
    public string descript;
    private GameObject player;
    private GameManagerScript gm;


    private void Start()
    {
        gm = GameManagerScript.instance();
    }

    /*             TILESCRIPT INHERITANCE                */

    // sending player to jail
    public override void Activate()
    {
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().SetLocIndex(10);
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().transform.position = gm.GetTile(10).transform.position;
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().SetStateToJail();
    }

    // Displaying tile info
    public override string TileInfo()
    {
        return descript;
    }
}
