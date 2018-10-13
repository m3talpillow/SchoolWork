using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

public class PropertyScript : TileScript, IBuyTile
{
    public int buyCost;
	public int[] rent;
    public int upgradeCost;
    public int mortgageValue;
    public GameObject[] linkedTiles;

    private int rentIndex;
	private bool isMortgaged;
    private GameObject owner;
    private GameManagerScript gm;

    public void Start()
    {
        gm = GameManagerScript.instance();
    }

    // Upgrade the property
    public void Upgrade(int index)
    {
        // If it can be upgraded
        if (rentIndex >= rent.Length - 1)
        {
            InfoScript.instance().Displayer("You cannot upgrade further.");
            return;
        }
        else if (isMortgaged == true)
        {
            InfoScript.instance().Displayer("Cannot upgrade mortgaged property.");
            return;
        }

        // If player has enough money
        if (owner.GetComponent<PlayerScript>().GetCash() < upgradeCost)
        {
            InfoScript.instance().Displayer("Player doesn't have enough money to upgrade.");
            return;
        }


        owner.GetComponent<PlayerScript>().RemvCash(upgradeCost);
        rentIndex++;
        gm.GetTileButton(index).GetComponentInChildren<Text>().text = rentIndex.ToString();
    }

    public void SellUpgrade(int index)
    {
        if (rentIndex <= 0)
        {
            InfoScript.instance().Displayer("There are no upgrades to sell.");
            return;
        }
        else if (isMortgaged == true)
        {
            InfoScript.instance().Displayer("Cannot sell upgrade on mortgaged property.");
            return;
        }

        // Give player cash
        owner.GetComponent<PlayerScript>().AddCash(upgradeCost / 2);

        rentIndex--;
        gm.GetTileButton(index).GetComponentInChildren<Text>().text = rentIndex.ToString();
    }




    /*             IBUYTILE INTERFACE                */

    //Is it owned?
    public bool IsOwned()
	{
		return (owner != null);
	}

	// Set the owner.
	public void SetOwner(GameObject player)
	{
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
        if (rentIndex != 0)
            InfoScript.instance().Displayer("You need to have no upgrades to mortgage.");

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

	// Is this property mortgaged?
	public bool GetIsMortgaged()
	{
		return isMortgaged;
	}

	// Return the propety's current rent.
	public int GetRent()
	{
		return rent[rentIndex];
	}

    // Return price of tile
    public int GetCost()
    {
        return buyCost;
    }

    /*             TILESCRIPT INHERITANCE                */

    // Buying process
    public override void Activate()
    {
        InfoScript.instance().Displayer(gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName() + " has landed on " + tileName);
         // If the tile doesn't have an owner, ask to buy
        if (owner == null)
        {
            //bool response = EditorUtility.DisplayDialog(tileName, ("Would you like to buy this tile for " + buyCost + "?"), "Yes", "No");
            StartCoroutine(ModalPopup.instance().Dialog("Buy this property?"));
            Invoke("restOfActivate", 4);
        }
    }

    private void restOfActivate()
    {
        bool? response = ModalPopup.response;

        // If they did want to buy
        if (response == true)
        {
            GameObject player = gm.GetComponent<GameManagerScript>().GetCurrentPlayer();
            int playerCash = player.GetComponent<PlayerScript>().GetCash();

            if (playerCash < buyCost)
            {
                InfoScript.instance().Displayer("You don't have enough money");
                return;
            }

            // Update player's cash and add to their ownership
            player.GetComponent<PlayerScript>().RemvCash(buyCost);
            player.GetComponent<PlayerScript>().GetOwnedTiles().Add(gameObject);
            player.GetComponent<PlayerScript>().IncNumProp();
            this.owner = player;
            InfoScript.instance().Displayer(gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName() + " has bought " + tileName + "!");
        }
        else
        {
            InfoScript.instance().Displayer(gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName() + "didn't buy" + tileName);
        }
    }

    // Displaying tile info
    public override string TileInfo()
    {
        string s = "Purchase cost: \n\t$" + buyCost + "\n\n";

        s += "Rent price: \n";
        for (int i = 0; i < rent.Length; i++)
            s += "\t$" + rent[i] + "\n";
        s += "\n";

        s += "Mortgage value: \n\t$" + mortgageValue + "\n\n";

        s += "Family tiles: \n";
        for (int i = 0; i < linkedTiles.Length; i++)
            s += "\t" + linkedTiles[i].GetComponent<TileScript>().GetName() + "\n";

        return s;
    }
}
