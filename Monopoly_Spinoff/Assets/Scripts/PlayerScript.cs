// Jared and Tyler
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

public class PlayerScript : MonoBehaviour
{
    // Player info
    public int cash;
    public int numProperties;
    public List<GameObject> ownedTiles;

    private string playerName;
    private int timeInJail;
    private int playerIndex;    // In gm's player array
    private int locationIndex;  // On gm's tile array
    private int oldIndex;       // used when moving player

    // State
    private enum State { Waiting, Active, Moving, Animation, OnTile, Jail, Finished };
    State state;

    // Movement
    private int lastRolled;
    private float waitTime;
    private float moveTime;
    private float elapsedTime;
    private Vector3 startPos;
    private Vector3 targetPos;

    // References
    private GameManagerScript gm;
    private DieScript die;
    private BoardScript board;


    // Init
    void Start()
    {
        // setting initial state (must be Active)
        state = State.Active;
        // reference to die
        die = DieScript.instance();
        // reference to board
        board = BoardScript.instance();
        // reference to the game manager
        gm = GameManagerScript.instance();

        // inital assigns
        // playerInfoText.text = "init";
        cash = 1500;
        moveTime = 0.25f;
        timeInJail = 0;
        elapsedTime = int.MaxValue;
    }



    /*             PLAYER STATES                */


    // Waiting state
    private void SetWaiting()
    {
        state = State.Waiting;
        // payMeButton.SetActive(true);
    }

    // Active state
    public void StartTurn()
    {
        gm.SetCurrentPlayerText(playerName);

        if (state == State.Jail)
        {
            Jail();
            return;
        }

        state = State.Active;
        waitTime = 1.00f;
        // rollButton.SetActive(true);
        // Enable trading
    }

    // Rolling state
    public void Roll()
    {
        // Only roll when its the players turn and they've waited
        if (state != State.Active || waitTime > 0)
            return;

        // Disable trading
        // payMeButton.SetActive(false);

        lastRolled = die.RollDie();
        gm.GetComponent<GameManagerScript>().SetLastPlayerIndex(playerIndex);

        // Check for triple doubles, go to jail
        if (gm.GetNumDoubles() >= 3)
        {
            locationIndex = 10;
            transform.position = gm.GetTile(10).transform.position;
            state = State.Jail;
            gm.NextTurn();
            return;
        }

        // Update location to trigger Update()
        locationIndex += lastRolled;

        //Pass GO, collect 200 dollars.
        if (locationIndex >= gm.GetNumTiles())
        {
            this.cash += gm.GetTile(0).GetComponent<GOScript>().GetGoValue();
            locationIndex -= gm.GetNumTiles();
        }

        state = State.Moving;
    }

    // Moving State
    void Update()
    {
        // Reduce time to wait before able to roll
        if (waitTime > 0)
            waitTime -= Time.deltaTime;

        if (state == State.Moving)
        {
            // finds how many tiles on board
            int len = gm.GetComponent<GameManagerScript>().tilesList.Length;

            // Checks if finished moving
            if (lastRolled <= 0)
            {
                state = State.OnTile;
                return;
            }
            else
                lastRolled--;

            // Find position of next tile (centered)
            startPos = transform.position;
            GameObject nextTile = gm.GetComponent<GameManagerScript>().tilesList[mod((locationIndex - lastRolled), len)];
            if (nextTile.transform.childCount > 0)
                targetPos = nextTile.transform.GetChild(1).position;
            else
                targetPos = nextTile.transform.position;

            // Reset time to move
            elapsedTime = 0;
            state = State.Animation;
        }

        if (state == State.OnTile)
            ActivateTile();

        if (state == State.Finished)
            EndTurn();
    }

    // Handle animations
    private void LateUpdate()
    {
        // Move player by time
        if (elapsedTime < moveTime)
        {
            transform.position = Vector3.Lerp(startPos, targetPos, (elapsedTime / moveTime));
            elapsedTime += Time.deltaTime;
        }
        else if (state == State.Animation)
            state = State.Moving;
    }

    // Tile specific occurences and/ or buying tiles
    private void ActivateTile()
    {
        // Find tile and activate it
        gm.GetComponent<GameManagerScript>().tilesList[locationIndex].GetComponent<TileScript>().Activate();

        if (state == State.Jail)
            gm.NextTurn();
        // If double, roll again
        else if (die.isDouble())
            state = State.Active;
        else
            state = State.Finished;
    }

    // If player is stuck in jail
    private void Jail()
    {
        if (timeInJail >= 3)
        {
            timeInJail = 0;
            RemvCash(50);
            state = State.Active;
            return;
        }

        // Buy out of jail
        StartCoroutine(ModalPopup.instance().Dialog("Would you like to buy your freedom for $50?"));
        Invoke("restOfActivate", 4);
    }

    private void restOfActivate()
    {
        bool? response = ModalPopup.response;
        if (response == true)
        {
            RemvCash(50);
            timeInJail = 0;
            state = State.Active;
            InfoScript.instance().Displayer("You're free, roll to advance.");
            return;
        }

        // Atempt to roll doubles
        lastRolled = die.RollDie();
        if (die.isDouble())
        {
            InfoScript.instance().Displayer("Rolled doubles and broke out of jail!");
            timeInJail = 0;
            // Update location to trigger Update()
            locationIndex += lastRolled;
            //impossible to hit go from this location
            state = State.Moving;
        }
        else
        {
            timeInJail++;
            InfoScript.instance().Displayer("Did not roll doubles, stay in jail!");
            gm.NextTurn();
        }
    }

    // Ends player turn
    private void EndTurn()
    {
        if (cash <= 0 && numProperties <= 0)
            InfoScript.instance().Displayer(playerName + " has lost the game!");

        SetWaiting();
        //button.SetActive(false); // Client side
        gm.NextTurn();
    }




    /*             ADDITIONAL METHODS                */


    // Request trade from another player, return true if accepted, false otherwise
    public bool RequestTrade(int playerReciever)
    {
        //person = GetComponent<PlayerPrefs>;
        //return person.RecieveTrade()

        //trade button disabled when not turn by gamemanager

        return false;

    }

    public bool RecieveTrade(int playerRequester)
    {
        //creates box in front of the parameter
        //yes / no
        //returns true/false
        //if true open trade method

        return false;
    }

    // ADD a value to the cash amount
    public void AddCash(int cash)
    {
        this.cash += cash;
    }

    // REMOVE a value from cash
    public void RemvCash(int cash)
    {
        if (this.cash - cash < 0)
            InfoScript.instance().Displayer("This player doesn't have enough money to pay");
        else
            this.cash -= cash;
    }

    public void UpdateText()
    {
        string s = playerName + " cash: $" + cash;
        s += "\n";
        s += "properties: " + numProperties;
        if (ownedTiles.Count != 0)
        {
            s += "\nOwned Properties:\n";
            foreach(GameObject tile in ownedTiles)
            {
                s += " " + tile.name + "\n";
            }
        }
    }

    // Called when populating player panel
    public string PlayerInfo()
    {
        string s = "Player > " + playerName + "\n\n";

        s = "Cash: \n\t$" + cash + "\n\n";

        s += "Owned properties: \n";
        for (int i = 0; i < numProperties; i++)
            s += "\t" + ownedTiles[i].GetComponent<TileScript>().GetName() + "\n";
        s += "\n";

        return s;
    }

    // Need for when modding negative numbers
    private int mod(int a, int b)
    {
        return (a % b + b) % b;
    }




    /*          GETTERS AND SETTERS           */

    // To be set once game starts.
    public void SetPlayerIndex(int index)
    {
        this.playerIndex = index;
    }

    public int GetPlayerIndex()
    {
        return playerIndex;
    }

    public int GetLocIndex()
    {
        return locationIndex;
    }

    public void SetLocIndex(int index)
    {
        locationIndex = index;
    }

    public void SetName(string name)
    {
        playerName = name;
    }

    public string GetName()
    {
        return playerName;
    }

    // Returns entire list of owned tiles
    public List<GameObject> GetOwnedTiles()
    {
        return ownedTiles;
    }

    public int GetIndex()
    {
        return playerIndex;
    }

    public int GetCash()
    {
        return this.cash;
    }

    // SET the cash to a value
    public void SetCash(int cash)
    {
        this.cash = cash;
    }

    public void IncNumProp()
    {
        numProperties++;
    }

    public void DecNumProp()
    {
        numProperties--;
    }

    public void SetStateToJail()
    {
        state = State.Jail;
    }
}
