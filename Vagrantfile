# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure(2) do |config|
  config.vm.box = "ubuntu/wily64"

  config.vm.provider "virtualbox" do |vb|
    # Increases memory to avoid compiler errors, due to excessive memory usage
    vb.memory = 1536
  end

  config.vm.provision "shell", path: "./.system-setup.sh"
end
