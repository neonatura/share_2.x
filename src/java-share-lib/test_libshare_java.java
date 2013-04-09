
public class test_libshare_java {
    public static void main(String argv[]) {

        try {
            System.loadLibrary("share");
        }
        catch (UnsatisfiedLinkError e) {
            System.out.println("Failed to load the library \"share\"");
            System.out.println(e.toString());
            System.exit(0);
        }

        System.out.println("\nRunning Java bindings test...\n");

        libshare.jshare.test_main();
    }
}
