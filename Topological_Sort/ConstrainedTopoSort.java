// Jared Johansson
// COP3503 - Fall 2017

import java.util.*;
import java.io.*;

// Custom node for directed graph ajacencyList
class Node
{
  // List of all nodes that this node leads to. Size O(k)
  private ArrayList<Node> pointsTo;
  // How many nodes need to be used in order to use this one
  private int prereqs;

  // Constructor to make address
  public Node()
  {
    prereqs = 0;
  }

  // Constructor when size is known to initalize arraylist
  public Node(int size)
  {
    this();
    this.pointsTo = new ArrayList<>(size);
  }

  // Initalize size of arraylist once size is discovered
  public void initList(int size)
  {
    if (pointsTo == null)
      this.pointsTo = new ArrayList<>(size);
  }

  // Adds nodes into Arraylist when initalizing
  public void addPointsTo(Node node)
  {
    pointsTo.add(node);
  }

  // Increment the number of prerequisits found
  public void incPre()
  {
    prereqs++;
  }

  // Get number of prerequisit nodes
  public int getPre()
  {
    return this.prereqs;
  }

  // Get node that this node points to at specified index
  public Node getIndex(int index)
  {
    return pointsTo.get(index);
  }

  // How many nodes this node points to
  public int numPoints()
  {
    return pointsTo.size();
  }
}

public class ConstrainedTopoSort
{
  // Ajacency List, size: O(n)
  private Node[] list;
  private int size;

  // Has ToP0sOrt?
  public boolean hasConstrainedTopoSort(int x, int y)
  {
    // Stack to handle which node to take next
    Stack<Node> stack = new Stack<>();
    // Hashmap to keep track of how many prerequisits needed
    HashMap<Node, Integer> counts = new HashMap<>(this.size);
    Boolean xfound = false, correct = false;
    int used = 0;

    // Load stack and counts, rt O(n)
    for (int i = 1; i <= this.size; i++)
    {
      // No prerequisits, good to go on stack and be used
      if (list[i].getPre() == 0)
        stack.push(list[i]);
      // Keep track of node and how many prereqs it has before use
      else
        counts.put(list[i], list[i].getPre());
    }

    // Start tracing a path, rt O(nk)
    while (!stack.empty())
    {
      // Get ready node
      Node current = stack.pop();

      // if x has been found and we can use y, success!
      if (list[y] == current && xfound == true)
        correct = true;

      // If x has not been passed, return y and grab next node
      else if (list[y] == current && xfound == false)
      {
        // Grab next value (if exisits!)
        try
        {
          Node next = stack.pop();
          stack.push(current);
          current = next;
        }
        // y value is only value on stack, not possible
        catch (EmptyStackException e)
        {
          return false;
        }
      }

      // Toggle boolean if x is passed
      if (list[x] == current)
        xfound = true;

      // Count number of nodes that have been succesfuly used.
      used++;

      // Decrement all the nodes current node points to, rt O(k)
      for (int i = 0; i < current.numPoints(); i++)
      {
        counts.put(current.getIndex(i), counts.get(current.getIndex(i)) - 1);

        // No more prerequisits, push onto stack
        if (counts.get(current.getIndex(i)) == 0)
          stack.push(current.getIndex(i));
      }
    }

    // If stack was emptied, correct order found, and all nodes used
    if (correct && used == this.size)
      return true;

    return false;
  }

  // Constructor to open and read in from file
  public ConstrainedTopoSort(String fileName)
  {
    try
    {
      Scanner scanner = new Scanner(new FileReader(fileName));
      // Initalize arraylist (first value is number of nodes)
      this.size = scanner.nextInt();
      this.list = new Node[size + 1];

      // Read in all nodes, rt O(nk)
      for (int nodeNum = 1; nodeNum <= size; nodeNum++)
      {
        // number of nodes current node will point to
        int numPoints = scanner.nextInt();

        // If current node net yet created/ initalized
        if (list[nodeNum] == null)
          list[nodeNum] = new Node(numPoints);
        else
          list[nodeNum].initList(numPoints);

        // Number of things current node points to
        for (int i = 0; i < numPoints; i++)
        {
          // Get the next node that this node points to
          int nextNode = scanner.nextInt();

          // Next node not yet created, make uninitalized node
          if (list[nextNode] == null)
            list[nextNode] = new Node();

          // Link next node to our nodes arrayList
          list[nodeNum].addPointsTo(list[nextNode]);
          // Incrments nexts nodes number of prerequisits
          list[nextNode].incPre();
        }
      }
    }
    // If file could not be found or other error, exit
    catch (Exception e)
    {
      e.printStackTrace();
      System.exit(0);
    }
  }

  // Mock print to see if constructor is working, runtime O(nk)
  public void printList()
  {
    for (int i = 1; i <= this.size; i++)
    {
      System.out.print("i:"+i+", "+list[i]+", pre:"+list[i].getPre()+", ");
      for (int j = 0; j < list[i].numPoints(); j++)
        System.out.print(list[i].getIndex(j)+" ");
      System.out.println();
    }
  }

  public static double difficultyRating()
  {
    return 2.0;
  }

  public static double hoursSpent()
  {
    return 5.0;
  }
}
