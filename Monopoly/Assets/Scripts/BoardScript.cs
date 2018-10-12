using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoardScript : MonoBehaviour {

    // The following block is singleton pattern, basically
    // it should give an access point to it from all gameobjects by calling
    // BoardScript.instance();
    // see http://gameprogrammingpatterns.com/singleton.html for details
    private static BoardScript instance_;
    private BoardScript(){}
    public static BoardScript instance()
    {
      if (instance_ == null)
          instance_ = GameObject.FindObjectOfType<BoardScript>();
      return instance_;
    }
    ////////////////////////////////


    private GameObject[] tiles;

    public GameObject GetTile(int index)
    {
        return tiles[index];
    }

    void Start()
    {

       tiles = GameObject.FindGameObjectsWithTag("Tile");
    }

    // Moves player avatar
    public void MovePlayerTo()
    {
        // Get player from gamemanger players array, move position by dice roll
    }

}
