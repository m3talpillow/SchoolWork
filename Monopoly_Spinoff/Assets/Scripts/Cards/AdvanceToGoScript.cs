using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AdvanceToGoScript : CardScript
{
    private GameManagerScript gm;
    
    public override void Activate()
    {
        InfoScript.instance().Displayer(cardName + "\n" + text);
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().SetLocIndex(0);
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().transform.position = gm.GetTile(0).transform.position;
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().AddCash(200);
        //make this a reference to go tile
    }
    public void Start()
    {
        gm = GameManagerScript.instance(); 
    }

}
