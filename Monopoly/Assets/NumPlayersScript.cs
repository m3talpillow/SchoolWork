using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NumPlayersScript : MonoBehaviour
{

    public int numPlayers;

    public void SetNumPlayers(int num)
    {
        numPlayers = num + 1;
    }

    public int GetNumPlayers()
    {
        return numPlayers;
    }
}
