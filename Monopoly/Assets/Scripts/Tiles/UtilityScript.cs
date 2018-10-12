//Matthew Drabek
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UtilityScript : TileScript, IBuyTile
{
    public int buyCost;
    public int mortgageValue;
    private int[] multiplier = { 4, 10 };
    private int multIndex;
    public GameObject[] linkedTiles;

    private bool isMortgaged;
    private GameObject owner;
    private GameManagerScript gm;

    public void Start()
    {
        gm = GameManagerScript.instance();
        multIndex = 0;
    }

    // Upgrade the property by updating its multiplier
    public void Upgrade()
    {
        if (multIndex < multiplier.Length - 1)
            multIndex++;
    }




    /*             IBUYTILE INTERFACE                */

    //Is it owned?
    public bool IsOwned()
	{
		return (owner != null);
	}

    // Set the owner and multipler amount
    public void SetOwner(GameObject player)
    {
        multIndex = 0;

        for (int i = 0; i < linkedTiles.Length; i++)
        {
            if (player == linkedTiles[i].GetComponent<IBuyTile>().GetOwner())
            {
                linkedTiles[i].GetComponent<UtilityScript>().Upgrade();
                Upgrade();
            }
        }

        owner = player;
    }

    // Return owner
    public GameObject GetOwner()
    {
        return owner;
    }

    //Pay the player when pay me button has been pressed
    public void PayPlayer(GameObject payer)
    {
        payer.GetComponent<PlayerScript>().RemvCash(GetRent());
        owner.GetComponent<PlayerScript>().AddCash(GetRent());
        InfoScript.instance().Displayer(payer.GetComponent<PlayerScript>().GetName() + " paid " + owner.GetComponent<PlayerScript>().GetName() + " " + GetRent());
    }

    // This property is now mortgaged.
    public void ToMortgaged()
    {
        owner.GetComponent<PlayerScript>().AddCash(mortgageValue);
        isMortgaged = true;
    }

    // This property is now not mortgaged.
    public void FromMortgaged()
    {
        owner.GetComponent<PlayerScript>().RemvCash(mortgageValue + (mortgageValue / 10));

        isMortgaged = false;
    }

    // Decides how to handle mortgaging
    public void Mortgage()
    {
        // Already mortgaged
        if (isMortgaged == true)
        {
            if (owner.GetComponent<PlayerScript>().GetCash() > mortgageValue + (mortgageValue / 10))
            {
                InfoScript.instance().Displayer("Bought back property from mortgage.");
                FromMortgaged();
                return;
            }
            else
            {
                InfoScript.instance().Displayer("You don't have enough money to buy from mortgage.");
                return;
            }
        }

        ToMortgaged();
    }

    //Is this property mortgaged?
    public bool GetIsMortgaged()
	{
		return isMortgaged;
	}

	//Return the propety's current rent (multiply by dice roll)
	public int GetRent()
	{
		return DieScript.instance().GetPrevDieRoll() * multiplier[multIndex];
	}

    // Return price of tile
    public int GetCost()
    {
        return buyCost;
    }

    // When trying to sell
    public void SellTile()
    {
        if (isMortgaged == false)
        {
            InfoScript.instance().Displayer("You're trying to mortgage " + tileName);
        }
        else
            InfoScript.instance().Displayer(tileName + " is already mortgaged!");
    }




    /*             TILESCRIPT INHERITANCE                */

    // Buying process
    public override void Activate()
    {
        InfoScript.instance().Displayer(gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName() + " has landed on " + tileName);
        // If the tile doesn't have an owner, ask to buy
        if (owner == null)
        {
            StartCoroutine(ModalPopup.instance().Dialog("Buy this property?"));
            Invoke("restOfActivate", 4);

        }
    }


    private void restOfActivate()
    {        // If they did want to buy

        bool? response = ModalPopup.response;

        if (response == true)
        {
            GameObject player = gm.GetComponent<GameManagerScript>().GetCurrentPlayer();
            int playerCash = player.GetComponent<PlayerScript>().GetCash();

            if (playerCash < buyCost)
            {
                InfoScript.instance().Displayer("You don't have enough money to buy.");
                return;
            }

            // Update player's cash and add to their ownership
            player.GetComponent<PlayerScript>().SetCash(playerCash - buyCost);
            player.GetComponent<PlayerScript>().GetOwnedTiles().Add(gameObject);
            player.GetComponent<PlayerScript>().IncNumProp();
            SetOwner(player);
            InfoScript.instance().Displayer(gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName() + " has bought " + tileName + "!");
        }
        else
        {
            InfoScript.instance().Displayer(gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName() + "didn't buy" + tileName);
            // auction
        }
    }

    // Displaying tile info
    public override string TileInfo()
    {
        string s = "Purchase cost: \n\t$" + buyCost + "\n\n";

        s += "Rent price: \n";
        for (int i = 0; i < multiplier.Length; i++)
            s += "\tLast roll * " + multiplier[i] + "\n";

        s += "Mortgage value: \n\t$" + mortgageValue + "\n\n";

        s += "Family tiles: \n";
        for (int i = 0; i < linkedTiles.Length; i++)
            s += "\t" + linkedTiles[i].GetComponent<TileScript>().GetName() + "\n";

        return s;
    }
}
