using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InfoScript : MonoBehaviour
{
	public Text log;
	public GameObject window;

	private static InfoScript instance_;
	private InfoScript() { }
	public static InfoScript instance()
	{
		if (instance_ == null)
			instance_ = GameObject.FindObjectOfType<InfoScript>();
		return instance_;
	}

	//Activates window, and displays the string.
	public void Displayer(string message)
	{
	    log.text = message;
		if (window.activeInHierarchy)
			window.GetComponent<TextPopup>().Multiply();
	    else
			window.SetActive(true);
	}
}
