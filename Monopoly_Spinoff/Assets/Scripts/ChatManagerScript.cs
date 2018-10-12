using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ChatManagerScript : MonoBehaviour
{
	public int maxMessages = 100;
	public GameObject chatPanel, textObject;
	public InputField chatBox;
	public Color playerMessage = Color.green;

	private string username;
	private GameManagerScript gm;

	[SerializeField]
	List<Message> messageList = new List<Message>();

	void Start()
	{ 
		playerMessage.a = 1;
		gm = GameManagerScript.instance();
	}

	void Update()
	{
		if(chatBox.text != "")
		{
			if(Input.GetKeyDown(KeyCode.Return))
			{
				username = gm.GetCurrentPlayer().GetComponent<PlayerScript>().GetName();
				SendMessageToChat(username + ": " + chatBox.text, Message.MessageType.playerMessage);
				chatBox.text = "";
			}
		}
		else
		{
			if(!chatBox.isFocused && Input.GetKeyDown(KeyCode.Return))
			{
				chatBox.ActivateInputField();
			}
		}
	}

	public void SendMessageToChat(string text, Message.MessageType messageType)
	{
		if(messageList.Count >= maxMessages)
		{
			Destroy(messageList[0].textObject.gameObject);
			messageList.Remove(messageList[0]);
		}

		Message newMessage = new Message();
		newMessage.text = text;
		GameObject newText = Instantiate(textObject, chatPanel.transform);

		newMessage.textObject = newText.GetComponent<Text>();
		newMessage.textObject.text = newMessage.text;
		newMessage.textObject.color = MessageTypeColor(messageType);

		messageList.Add(newMessage);
	}

	Color MessageTypeColor(Message.MessageType messageType)
	{
		Color color = playerMessage;

		switch(messageType)
		{
			case Message.MessageType.playerMessage:
				color = playerMessage;
				break;
		}

		return color;
	}
}
	
[System.Serializable]
public class Message
{
	public string text;
	public Text textObject;
	public MessageType messageType;

	public enum MessageType
	{
		playerMessage
	}
}