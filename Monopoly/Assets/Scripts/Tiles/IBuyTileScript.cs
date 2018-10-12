
﻿//Matthew Drabek
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public interface IBuyTile
{
	bool IsOwned();
    GameObject GetOwner();
	void SetOwner(GameObject player);

	void PayPlayer(GameObject payer);

	void ToMortgaged();
    void FromMortgaged();
	bool GetIsMortgaged();
    void Mortgage();

    int GetCost();
	int GetRent();
}
