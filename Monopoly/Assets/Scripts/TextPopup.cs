using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TextPopup : MonoBehaviour
{

	// just removes popup after a few seconds

	private float timer;
	private bool disable;
	int instances;

	void Awake()
	{
		disable = false;
	}

	void Update()
	{
		timer -= Time.deltaTime;
		if(timer < 0)
		{
			if(disable == true)
				DisableParent();
			disable = false;
			gameObject.SetActive(false);
		}
	}

	public void Multiply()
	{
		instances++;
		timer = 1.5f * instances;
	}

	public void OnEnable()
	{
		Multiply();
	}

	public void OnDisable()
	{
		instances = 0;
	}

	public void DisableMe()
	{
		disable = true;
	}

	public void DisableParent()
	{
		transform.parent.gameObject.SetActive(false);
	}
}
