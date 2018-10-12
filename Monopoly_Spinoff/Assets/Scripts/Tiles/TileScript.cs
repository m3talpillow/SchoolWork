// Joe and Jared
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class TileScript : MonoBehaviour {

    public string tileName;

    // wrapper/overridden method for each type of tile to activate their
    // particular behaviour
    public abstract void Activate();
    // To be implemented when clicking on the tile
    public abstract string TileInfo();

    // get name of tile
    public string GetName()
    {
        return tileName;
    }
}
