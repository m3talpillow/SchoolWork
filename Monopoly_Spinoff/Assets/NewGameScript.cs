using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class NewGameScript : MonoBehaviour
{

    public GameObject NumPlayers;

    void Start()
    {
        NumPlayers = GameObject.Find("NumPlayers");
    }

    public void Transition()
    {
        DontDestroyOnLoad(NumPlayers);
        SceneManager.LoadScene("Main");
    }
}