public class main{
    public static void main(String [] args){
        Employee.displayEmpCount();
        Employee emp1 = new Employee("biruk");
        Employee emp2 = new Employee("kebede");
        Employee emp3 = new Employee("kebede");
        Employee.displayEmpCount();
        System.out.println("Emp 1 Name: " + emp1.getName());
    }
}