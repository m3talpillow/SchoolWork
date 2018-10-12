using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DieScript : MonoBehaviour
{
    // The following block is singleton pattern, basically
    // it should give an access point to it from all gameobjects by calling
    // DieScript.instance();
    // see http://gameprogrammingpatterns.com/singleton.html for details
    private static DieScript instance_;
    private DieScript() { }
    public static DieScript instance()
    {
        if (instance_ == null)
            instance_ = GameObject.FindObjectOfType<DieScript>();
        return instance_;
    }
    ////////////////////////////////

    private GameManagerScript gm;
    private const int STANDARD_SIDES = 6;
    private int die1 = 0;
    private int die2 = 0;

    void Start()
    {
        // reference to the game manager
        gm = GameManagerScript.instance();
    }

    // method overload to handle standart sided die rolls
    public int RollDie()
    {
        return RollDie(STANDARD_SIDES);
    }

    // Rolls two dice, upadtes previous die roll and possibly calls incDouble in gamemanager
    public int RollDie(int numSides)
    {
        die1 = Random.Range(1, numSides + 1);
        die2 = Random.Range(1, numSides + 1);
        if (isDouble())
            gm.IncDouble();

        InfoScript.instance().Displayer("Rolled: " + (die1 + die2) + (isDouble() ? " DOUBLE" : ""));

        return die1 + die2;
    }

    // Checks if double and sets bool
    public bool isDouble()
    {
        return die1 == die2;
    }

    // Returns combined value of dices rolls
    public int GetPrevDieRoll()
    {
        return die1 + die2;
    }    
}
