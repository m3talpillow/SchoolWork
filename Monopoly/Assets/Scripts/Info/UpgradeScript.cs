using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UpgradeScript : MonoBehaviour
{
    private GameManagerScript gm;
    private int lastTileIndex;

    private void Start()
    {
        gameObject.SetActive(true);
        gm = GameManagerScript.instance();
    }

    public void SetIndex(int index)
    {
        lastTileIndex = index;
    }

    // if current player owns property
    public bool verifyOwner()
    {
        try
        {
            List<GameObject> tiles = gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetOwnedTiles();
            GameObject selectedTile = gm.GetTile(lastTileIndex);

            if (tiles.Contains(selectedTile) == false)
            {
                InfoScript.instance().Displayer("You do not own this tile!");
                return false;
            }

            return true;
        }
        catch (Exception e)
        {
            return false;
        }  
    }

    // Method the sell button calls
    public void PressSellButton()
    {
        if (verifyOwner() == false)
            return;

        try
        {
            InfoScript.instance().Displayer("Sold upgrade on " + gm.GetComponent<GameManagerScript>().GetTile(lastTileIndex).GetComponent<TileScript>().GetName());
            gm.GetComponent<GameManagerScript>().GetTile(lastTileIndex).GetComponent<PropertyScript>().SellUpgrade(lastTileIndex);
        }
        catch (Exception e)
        {
            InfoScript.instance().Displayer("You cannot sell upgrades on this tile.");
        }
    }

    // Upgrade prop
    public void PressUpgradeButton()
    {
        if (verifyOwner() == false)
            return;

        try
        {
            InfoScript.instance().Displayer("Bought upgrade on " + gm.GetComponent<GameManagerScript>().GetTile(lastTileIndex).GetComponent<TileScript>().GetName());
            gm.GetComponent<GameManagerScript>().GetTile(lastTileIndex).GetComponent<PropertyScript>().Upgrade(lastTileIndex);
        }
        catch (Exception e)
        {
            InfoScript.instance().Displayer("This tile cannot be upgraded!");
        }
    }

    public void PressMortgageButton()
    {
        if (verifyOwner() == false)
            return;

        try
        {
            InfoScript.instance().Displayer("Mortgaged " + gm.GetComponent<GameManagerScript>().GetTile(lastTileIndex).GetComponent<TileScript>().GetName());
            gm.GetComponent<GameManagerScript>().GetTile(lastTileIndex).GetComponent<IBuyTile>().Mortgage();
        }
        catch (Exception e)
        {
            InfoScript.instance().Displayer("This tile cannot be mortgaged!");
        }
    }
}
