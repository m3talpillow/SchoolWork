using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BankErrorScript : CardScript
{
    private GameManagerScript gm;

    public override void Activate()
    {
        InfoScript.instance().Displayer(cardName + "\n" + text);
        gm.GetCurrentPlayer().GetComponent<PlayerScript>().AddCash(200);
    }

    private void Start()
    {
        gm = GameManagerScript.instance();
    }

}
