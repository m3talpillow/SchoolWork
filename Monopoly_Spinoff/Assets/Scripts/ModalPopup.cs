using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using UnityEngine;

public class ModalPopup : MonoBehaviour {

	public static bool? response = null;

	private static ModalPopup instance_;
    private ModalPopup() { }
    public static ModalPopup instance()
    {
        if (instance_ == null)
            instance_ = GameObject.FindObjectOfType<ModalPopup>();
        return instance_;
    }

	public static bool ended;

	public void Start()
	{
		ended = false;
	}

	public bool? ReturnResult()
	{
		return response;
	}

	int times = 0;
	public IEnumerator Dialog(string content)
	{
		if(times == 0)
		{
			StartCoroutine(waitYo());
			transform.GetChild(0).gameObject.SetActive(true);
			transform.GetChild(0).GetChild(1).GetChild(0).GetComponent<Text>().text = content;
			times = 1;
		}
		while(response == null) { yield return null; }
		transform.GetChild(0).gameObject.SetActive(false);
		ended = true;
		yield return response;
	}

	IEnumerator waitYo()
     {
         yield return new WaitForSeconds(5);
		 this.transform.GetChild(0).gameObject.SetActive(false);
		 Invoke("Reset", 6);
		 times = 0;
	 }

	private void Reset()
	{
		response = null;
	}

	public void Decision(int n)
	{
		if(n == 1)
			response = true;
		else
			response = false;
		Invoke("Reset", 6);
		times = 0;
		this.transform.GetChild(0).gameObject.SetActive(false);
	}
}
