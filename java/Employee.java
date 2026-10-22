public class Employee{
	public static int empCount = 0;
	private String name;
	public Employee(String name){
		this.name = name;
		empCount++;
	}
	public static void displayEmpCount(){
		System.out.println("Total active Employee: " + empCount);
	}
	public String getName(){
		return name;
	}
}
