using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GrandOperaNight : CardScript {

    private GameManagerScript gm;

    public override void Activate()
    {
        InfoScript.instance().Displayer(cardName + "\n" + text);
        int x = 0;
        //take 50 from every player and give to current player

        gm.GetCurrentPlayer().GetComponent<PlayerScript>().AddCash(x);
    }

    private void Start()
    {
        gm = GameManagerScript.instance();
    }
}
