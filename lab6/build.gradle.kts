plugins {
    id("java")
    id("application")
}

allprojects {
    group = "org.tesserakt"
    version = "1.0-SNAPSHOT"

    apply(plugin = "java")
    apply(plugin = "application")

    repositories {
        mavenCentral()
    }

    dependencies {
        implementation("org.jacorb:jacorb-omgapi:3.9")
        implementation("org.jacorb:jacorb:3.9")
    }

    tasks.test {
        useJUnitPlatform()
    }
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(17))
    }
}

project(":server") {
    dependencies {
        implementation(project(":shared"))
        implementation("org.slf4j:slf4j-simple:2.0.5")
    }

    application {
        mainClass.set("${group}.Main")
    }
}

project(":client") {
    dependencies {
        implementation(project(":shared"))
        implementation("info.picocli:picocli:4.7.3")
        implementation("org.slf4j:slf4j-simple:2.0.5")
        runtimeOnly("org.jboss.spec.javax.rmi:jboss-rmi-api_1.0_spec:1.0.6.Final")
    }

    application {
        mainClass.set("${group}.Main")
    }
}