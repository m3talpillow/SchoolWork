using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StockScript : CardScript {

    private GameManagerScript gm;

    public override void Activate()
    {
        InfoScript.instance().Displayer(cardName + "\n" + text);
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().AddCash(50);
    }

    private void Start()
    {
        gm = GameManagerScript.instance();
    }
}
