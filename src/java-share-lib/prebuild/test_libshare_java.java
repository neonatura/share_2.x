
public class test_libshare_java {
    public static void main(String argv[]) {

/*
        try {
            System.loadLibrary("share_java");
        }
        catch (UnsatisfiedLinkError e) {
            System.out.println("Failed to load the library \"share\"");
            System.out.println(e.toString());
            System.exit(0);
        }
*/

        System.out.println("libshare version 2.25 (libshare)");
        System.out.println("Java Library Version: " + net.sharelib.share_java.get_libshare_version());

        System.exit(net.sharelib.share_java.test_main());
    }
}
