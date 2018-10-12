using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerInfoButtonScript : MonoBehaviour
{
    public Text text;
    private GameObject player;
    private GameManagerScript gm;

    void Start()
    {
        gm = GameManagerScript.instance();
    }

    public void SetPlayer(GameObject p)
    {
        player = p;
    }

    public GameObject GetPlayer()
    {
        return player;
    }

    public void WriteToPlayerPanel()
    {
        gm.info_player = player;
        text.text = player.GetComponent<PlayerScript>().PlayerInfo();
    }
}
